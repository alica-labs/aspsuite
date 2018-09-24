/*
 * ASPSolver.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#include <asp_solver/ASPSolver.h>

#include <asp_commons/AnnotatedValVec.h>
#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>
#include <asp_solver/AnnotatedExternal.h>
#include <asp_solver/ASPFactsQuery.h>
#include <asp_commons/ASPQuery.h>
#include <asp_solver/ASPVariableQuery.h>
#include <asp_commons/ASPCommonsVariable.h>

namespace reasoner
{

	mutex ASPSolver::queryCounterMutex;

	ASPSolver::ASPSolver(vector<char const*> args) :
			IASPSolver()
	{
		//this->gringoModule = new DefaultGringoModule();
//		this->clingo = this->gringoModule->newControl(args.size() - 2, args.data(), nullptr, 20);
		this->clingo = make_shared<Clingo::Control>(args.data(), logger, 20);
		this->disableWarnings(true);

		this->sc = supplementary::SystemConfig::getInstance();
		this->queryCounter = 0;
#ifdef ASPSolver_DEBUG
		this->modelCount = 0;
#endif
		this->conf = this->clingo->configuration();
		//this->root = this->conf->getRootKey();
		this->modelsKey = this->conf["solve.models"]; //->getSubKey(this->root, "solve.models");
		this->modelsKey = "0";
		//this->conf->setKeyValue(this->modelsKey, "0");
#ifdef SOLVER_OPTIONS
		traverseOptions(conf, root, "");
#endif
	}

	ASPSolver::~ASPSolver()
	{
//		delete this->gringoModule;
//		delete this->clingo;

	}

	void ASPSolver::loadFile(string absolutFilename)
	{
		this->clingo->load(absolutFilename.c_str());
	}

	bool ASPSolver::loadFileFromConfig(string configKey)
	{

		if (configKey.empty())
		{
			return true;
		}
		for (auto file : this->alreadyLoaded)
		{
			if (configKey.compare(file) == 0)
			{
				return false;
			}
		}

		string backGroundKnowledgeFile = (*this->sc)["ASPSolver"]->get<string>(configKey.c_str(), NULL);
		this->alreadyLoaded.push_back(configKey.c_str());
		backGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*this->sc).getConfigPath(),
																			backGroundKnowledgeFile);
		this->clingo->load(backGroundKnowledgeFile.c_str());
		return true;
	}

	/**
	 * Let the internal solver ground a given program part (context).
	 */
	void ASPSolver::ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack)
	{
#ifdef ASPSOLVER_DEBUG
		cout << "ASPSolver_ground: " << vec.at(0).first << endl;
#endif
		this->clingo->ground(vec, callBack);
	}

	/**
	 * Let the internal solver solve one time.
	 */
	bool ASPSolver::solve()
	{
		this->currentModels.clear();
		this->reduceQueryLifeTime();
#ifdef ASPSolver_DEBUG
		this->modelCount = 0;
#endif
		auto result = this->clingo->solve({}, bind(&ASPSolver::onModel, this, placeholders::_1));
		return result.satisfiable();
	}

	/**
	 * Callback for created models during solving.
	 */
	bool ASPSolver::onModel(const Clingo::Model& m)
	{
#ifdef ASPSolver_DEBUG
		cout << "ASPSolver: Found the following model which is number " << endl;
		for (auto &atom : m.atoms(Gringo::Model::SHOWN))
		{
			cout << atom << " ";
		}
		cout << endl;
#endif
		Clingo::Model& clingoModel = (Clingo::Model&)m;
		Clingo::SymbolVector vec;
		auto tmp = clingoModel.symbols(clingo_show_type_shown);
		for (int i = 0; i < tmp.size(); i++)
		{
			vec.push_back(tmp[i]);
		}
		this->currentModels.push_back(vec);
		for (auto& query : this->registeredQueries)
		{
			query->onModel(clingoModel);
		}
		return true;
	}

	void ASPSolver::assignExternal(Clingo::Symbol ext, Clingo::TruthValue truthValue)
	{
		this->clingo->assign_external(ext, truthValue);
	}

	void ASPSolver::releaseExternal(Clingo::Symbol ext)
	{
//		this->clingo->assignExternal(ext, Potassco::Value_t::False);
		//TODO test was free before
		this->clingo->release_external(ext);
	}

	void ASPSolver::add(char const *name, const Clingo::StringSpan& params, char const *par)
	{
		this->clingo->add(name, params, par);
	}

	Clingo::Symbol ASPSolver::parseValue(const std::string& str)
	{
		return this->gringoModule->parseValue(str, nullptr, 20);
	}

	bool ASPSolver::registerQuery(shared_ptr<ASPQuery> query)
	{
		auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
		if (entry == this->registeredQueries.end())
		{
			this->registeredQueries.push_back(query);
			return true;
		}
		return false;
	}

	bool ASPSolver::unregisterQuery(shared_ptr<ASPQuery> query)
	{
		query->removeExternal();
		auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
		if (entry != this->registeredQueries.end())
		{
			this->registeredQueries.erase(entry);
			return true;
		}
		return false;
	}

	void ASPSolver::disableWarnings(bool disable)
	{
		this->logger.enable(Clingo::WarningCode::AtomUndefined, !disable);
		this->logger.enable(Clingo::WarningCode::FileIncluded, !disable);
		this->logger.enable(Clingo::WarningCode::GlobalVariable, !disable);
		this->logger.enable(Clingo::WarningCode::VariableUnbounded, !disable);
		this->logger.enable(Clingo::WarningCode::Other, !disable);
		this->logger.enable(Clingo::WarningCode::OperationUndefined, !disable);
		this->logger.enable(Clingo::WarningCode::RuntimeError, !disable);
	}

	bool ASPSolver::existsSolution(vector<shared_ptr<ASPCommonsVariable>>& vars,
									vector<shared_ptr<ASPCommonsTerm>>& calls)
	{

		//this->conf->setKeyValue(this->modelsKey, "1");
		this->modelsKey = "1";
		int dim = prepareSolution(vars, calls);
		if (dim == -1)
		{
			return false;
		}
		auto satisfied = this->solve();
		this->removeDeadQueries();
		return satisfied;
	}

	bool ASPSolver::getSolution(vector<shared_ptr<ASPCommonsVariable>>& vars, vector<shared_ptr<ASPCommonsTerm>>& calls,
								vector<void*>& results)
	{

        //this->conf->setKeyValue(this->modelsKey, "0");
        this->modelsKey = "0";
		int dim = prepareSolution(vars, calls);
		if (dim == -1)
		{
			this->removeDeadQueries();
			return false;
		}
		auto satisfied = this->solve();
		if (!satisfied)
		{
			this->removeDeadQueries();
			return false;
		}
		for (auto& queriedId : this->currentQueryIds)
		{
			for (auto& query : this->registeredQueries)
			{
				vector<Clingo::SymbolVector> vals;
				for (auto& pair : query->getHeadValues())
				{
					vals.push_back(pair.second);
				}
				if (queriedId == query->getTerm()->getId())
				{
					results.push_back(new AnnotatedValVec(query->getTerm()->getId(), vals, query));
					break;
				}
			}
		}
		this->removeDeadQueries();
		this->currentQueryIds.clear();
		if (results.size() > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int ASPSolver::prepareSolution(vector<shared_ptr<ASPCommonsVariable>>& vars,
									vector<shared_ptr<ASPCommonsTerm>>& calls)
	{

		auto cVars = make_shared<vector<shared_ptr<reasoner::ASPCommonsVariable> > >(vars.size());
		for (int i = 0; i < vars.size(); ++i)
		{
			cVars->at(i) = vars.at(i);
		}
		vector<shared_ptr<reasoner::ASPCommonsTerm> > constraint;
		for (auto& c : calls)
		{
			constraint.push_back(c);
		}
		for (auto term : constraint)
		{
			this->currentQueryIds.push_back(term->getId());
			if (!term->getNumberOfModels().empty())
			{
				//this->conf->setKeyValue(this->modelsKey, term->getNumberOfModels().c_str());
                this->modelsKey = term->getNumberOfModels().c_str();
			}
			if (term->getType() == ASPQueryType::Variable)
			{
				if (this->registeredQueries.size() == 0)
				{
					this->registerQuery(make_shared<ASPVariableQuery>(this, term));
				}
				else
				{
					bool found = false;
					for (auto query : this->registeredQueries)
					{
						if (term->getId() == query->getTerm()->getId())
						{
							found = true;
						}
					}
					if (!found)
					{
						this->registerQuery(make_shared<ASPVariableQuery>(this, term));
					}
				}
			}
			else if (term->getType() == ASPQueryType::Facts)
			{
				bool found = true;
				for (auto query : this->registeredQueries)
				{
					if (term->getId() != query->getTerm()->getId())
					{
						found = false;
						break;
					}
				}
				if (!found)
				{
					this->registerQuery(make_shared<ASPFactsQuery>(this, term));
				}
			}
			else
			{
				cout << "ASPSolver: Query of unknown type registered!" << endl;
				return -1;
			}
			if (term->getExternals() != nullptr)
			{
				for (auto p : *term->getExternals())
				{
					auto it = find_if(this->assignedExternals.begin(), this->assignedExternals.end(),
										[p](shared_ptr<AnnotatedExternal> element)
										{	return element->getAspPredicate() == p.first;});
					if (it == this->assignedExternals.end())
					{
						shared_ptr<Clingo::Symbol> val = make_shared<Clingo::Symbol>(p.first, nullptr, 20);
						this->clingo->assign_external(*val,
														p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
						this->assignedExternals.push_back(make_shared<AnnotatedExternal>(p.first, val, p.second));
					}
					else
					{
						if (p.second != (*it)->getValue())
						{
							this->clingo->assign_external(
									*((*it)->getGringoValue()),
									p.second ? Clingo::TruthValue::True : Clingo::TruthValue::False);
							(*it)->setValue(p.second);
						}
					}
				}
			}
		}
		return vars.size();
	}

	shared_ptr<ASPCommonsVariable> ASPSolver::createVariable(long id)
	{
		return make_shared<reasoner::ASPCommonsVariable>();
	}

	void ASPSolver::removeDeadQueries()
	{
		// TODO: Optimize
		vector<shared_ptr<ASPQuery>> toRemove;
		for (auto query : this->registeredQueries)
		{
			if (query->getLifeTime() == 0)
			{
				toRemove.push_back(query);
			}
		}
		for (auto query : toRemove)
		{
			this->unregisterQuery(query);
		}
	}

	const void* ASPSolver::getWildcardPointer()
	{
		return WILDCARD_POINTER;
	}

	const string& ASPSolver::getWildcardString()
	{
		return WILDCARD_STRING;
	}

	int ASPSolver::getRegisteredQueriesCount()
	{
		return this->registeredQueries.size();
	}

	void ASPSolver::reduceQueryLifeTime()
	{
		for (auto query : this->registeredQueries)
		{
			query->reduceLifeTime();
		}
	}

	const double ASPSolver::getSolvingTime()
	{
        auto statistics = this->clingo->statistics();

		// time in seconds
		return statistics["time_solve"] * 1000;
	}

	const double ASPSolver::getSatTime()
	{
        auto statistics = this->clingo->statistics();

		// time in seconds
		return statistics["time_sat"] * 1000;
	}

	const double ASPSolver::getUnsatTime()
	{
        auto statistics = this->clingo->statistics();

		// time in seconds
		return statistics["time_unsat"]* 1000;
	}

	const double ASPSolver::getModelCount()
	{
        return this->clingo->statistics()["summary"]["lp"]["numEnum"];
	}

	int ASPSolver::getQueryCounter()
	{
		lock_guard<mutex> lock(queryCounterMutex);

		this->queryCounter++;
		return this->queryCounter;
	}

	const double ASPSolver::getAtomCount()
	{
        return this->clingo->statistics()["problem"]["lp"]["atoms"];
	}

	const double ASPSolver::getBodiesCount()
	{
        return this->clingo->statistics()["problem"]["lp"]["bodies"];
	}

	const double ASPSolver::getAuxAtomsCount()
	{
        return this->clingo->statistics()["problem"]["lp"]["aux_atoms"];
	}

	vector<Clingo::SymbolVector> ASPSolver::getCurrentModels()
	{
		return this->currentModels;
	}

	void ASPSolver::printStats()
	{
		auto statistics = this->clingo->statistics();

		stringstream ss;
		ss << "Solve Statistics:" << endl;
		ss << "TOTAL Time: " << statistics["time_total"] << "s" << endl;
		ss << "CPU Time: " << statistics["time_cpu"] << "s" << endl;
		ss << "SAT Time: " << (statistics["time_sat"] * 1000.0) << "ms" << endl;
		ss << "UNSAT Time: " << (statistics["time_unsat"] * 1000.0) << "ms" << endl;
		ss << "SOLVE Time: " << (statistics["time_solve"] * 1000.0) << "ms" << endl;

		cout << ss.str() << flush;
	}

	vector<shared_ptr<ASPQuery>> ASPSolver::getRegisteredQueries()
	{
		return registeredQueries;
	}

	/*DefaultGringoModule* ASPSolver::getGringoModule()
	{
		return gringoModule;
	}*/

} /* namespace reasoner */


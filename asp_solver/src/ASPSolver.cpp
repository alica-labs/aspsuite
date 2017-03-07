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
		this->gringoModule = new DefaultGringoModule();
//		this->clingo = this->gringoModule->newControl(args.size() - 2, args.data(), nullptr, 20);
		this->clingo = make_shared<ClingoLib>(this->gringoModule->scripts, args.size() - 2, args.data(), nullptr , 20);
		this->disableWarnings(true);

		this->sc = supplementary::SystemConfig::getInstance();
		this->queryCounter = 0;
#ifdef ASPSolver_DEBUG
		this->modelCount = 0;
#endif
		this->conf = &this->clingo->getConf();
		this->root = this->conf->getRootKey();
		this->modelsKey = this->conf->getSubKey(this->root, "solve.models");
		this->conf->setKeyValue(this->modelsKey, "0");
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
		this->clingo->load(absolutFilename);
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
		this->clingo->load(backGroundKnowledgeFile);
		return true;
	}

	/**
	 * Let the internal solver ground a given program part (context).
	 */
	void ASPSolver::ground(Gringo::Control::GroundVec const &vec, Gringo::Context *context)
	{
#ifdef ASPSOLVER_DEBUG
		cout << "ASPSolver_ground: " << vec.at(0).first << endl;
#endif
		this->clingo->ground(vec, context);
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
		auto result = this->clingo->solve(bind(&ASPSolver::onModel, this, placeholders::_1), {});
		if (result.satisfiable() == Gringo::SolveResult::Satisfiabily::Satisfiable)
		{
			return true;
		}
		return false;
	}

	/**
	 * Callback for created models during solving.
	 */
	bool ASPSolver::onModel(const Gringo::Model& m)
	{
#ifdef ASPSolver_DEBUG
		cout << "ASPSolver: Found the following model which is number " << endl;
		for (auto &atom : m.atoms(Gringo::Model::SHOWN))
		{
			cout << atom << " ";
		}
		cout << endl;
#endif
		ClingoModel& clingoModel = (ClingoModel&)m;
		Gringo::SymVec vec;
		auto tmp = clingoModel.atoms(clingo_show_type_shown);
		for(int i = 0; i < tmp.size; i++)
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

	void ASPSolver::assignExternal(Gringo::Symbol ext, Potassco::Value_t truthValue)
	{
		this->clingo->assignExternal(ext, truthValue);
	}

	void ASPSolver::releaseExternal(Gringo::Symbol ext)
	{
		this->clingo->assignExternal(ext, Potassco::Value_t::False);
		this->clingo->assignExternal(ext, Potassco::Value_t::Free);
	}

	void ASPSolver::add(const string& name, const Gringo::FWStringVec& params, const string& par)
	{
		this->clingo->add(name, params, par);
	}

	Gringo::Symbol ASPSolver::parseValue(const std::string& str)
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
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_atom_undefined, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_file_included, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_global_variable, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_variable_unbounded, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_other, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_operation_undefined, !disable);
		this->clingo->logger().enable(Gringo::Warnings::clingo_warning_runtime_error, !disable);
	}

	bool ASPSolver::existsSolution(vector<shared_ptr<ASPCommonsVariable>>& vars,
									vector<shared_ptr<ASPCommonsTerm>>& calls)
	{

		this->conf->setKeyValue(this->modelsKey, "1");
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

		this->conf->setKeyValue(this->modelsKey, "0");
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
		for (auto query : this->registeredQueries)
		{
			vector<Gringo::SymVec> vals;
			for (auto pair : query->getHeadValues())
			{
				vals.push_back(pair.second);
			}

			results.push_back(new AnnotatedValVec(query->getTerm()->getId(), vals, query));
		}
		if (results.size() > 0)
		{
			this->removeDeadQueries();
			return true;
		}
		else
		{
			this->removeDeadQueries();
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
			if (!term->getNumberOfModels().empty())
			{
				this->conf->setKeyValue(this->modelsKey, term->getNumberOfModels().c_str());
			}
			if (term->getType() == ASPQueryType::Variable)
			{
				this->registerQuery(make_shared<ASPVariableQuery>(this, term));
			}
			else if (term->getType() == ASPQueryType::Facts)
			{
				this->registerQuery(make_shared<ASPFactsQuery>(this, term));
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

						shared_ptr<Gringo::Symbol> val = make_shared<Gringo::Symbol>(
								this->gringoModule->parseValue(p.first, nullptr, 20));
						this->clingo->assignExternal(*val,
														p.second ? Potassco::Value_t::True : Potassco::Value_t::False);
						this->assignedExternals.push_back(make_shared<AnnotatedExternal>(p.first, val, p.second));
					}
					else
					{
						if (p.second != (*it)->getValue())
						{
							this->clingo->assignExternal(
									*((*it)->getGringoValue()),
									p.second ? Potassco::Value_t::True : Potassco::Value_t::False);
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

	const long long ASPSolver::getSolvingTime()
	{
		auto clasp = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (clasp == nullptr)
			return -1;

		// time in seconds
		return clasp->summary().solveTime * 1000;
	}

	const long long ASPSolver::getSatTime()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return -1;

		// time in seconds
		return claspFacade->summary().satTime * 1000;
	}

	const long long ASPSolver::getUnsatTime()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return -1;

		// time in seconds
		return claspFacade->summary().unsatTime * 1000;
	}

	const long ASPSolver::getModelCount()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return -1;

		return claspFacade->summary().numEnum;
	}

	int ASPSolver::getQueryCounter()
	{
		lock_guard<mutex> lock(queryCounterMutex);

		this->queryCounter++;
		return this->queryCounter;
	}

	const long ASPSolver::getAtomCount()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->statistics();

		if (claspFacade == nullptr)
			return -1;

		return claspFacade->summary().lpStats()->atoms;
	}

	const long ASPSolver::getBodiesCount()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return -1;

		return claspFacade->summary().lpStats()->bodies[1].numKeys();
	}

	const long ASPSolver::getAuxAtomsCount()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return -1;

		return claspFacade->summary().lpStats()->auxAtoms;
	}

	vector<Gringo::SymVec> ASPSolver::getCurrentModels()
	{
		return this->currentModels;
	}

	void ASPSolver::printStats()
	{
		auto claspFacade = (Clasp::ClaspFacade*)this->clingo->claspFacade();

		if (claspFacade == nullptr)
			return;

		stringstream ss;
		ss << "Solve Statistics:" << endl;
		ss << "TOTAL Time: " << claspFacade->summary().totalTime << "s" << endl;
		ss << "CPU Time: " << claspFacade->summary().cpuTime << "s" << endl;
		ss << "SAT Time: " << (claspFacade->summary().satTime * 1000.0) << "ms" << endl;
		ss << "UNSAT Time: " << (claspFacade->summary().unsatTime * 1000.0) << "ms" << endl;
		ss << "SOLVE Time: " << (claspFacade->summary().solveTime * 1000.0) << "ms" << endl;

		cout << ss.str() << flush;
	}

	vector<shared_ptr<ASPQuery>> ASPSolver::getRegisteredQueries()
	{
		return registeredQueries;
	}

} /* namespace reasoner */


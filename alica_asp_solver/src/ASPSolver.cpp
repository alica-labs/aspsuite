/*
 * ASPSolver.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPSolver.h"
#include "engine/model/Plan.h"
#include "engine/model/Variable.h"
#include <AutoDiff.h>

namespace alica
{
	namespace reasoner
	{

		const void* const ASPSolver::WILDCARD_POINTER = new int(0);
		const string ASPSolver::WILDCARD_STRING = "wildcard";

		ASPSolver::ASPSolver(AlicaEngine* ae, std::vector<char const*> args) :
				IConstraintSolver(ae), gen(ASPSolver::WILDCARD_POINTER, ASPSolver::WILDCARD_STRING)
		{
			this->gringoModule = new DefaultGringoModule();
			this->clingo = make_shared<ClingoLib>(*gringoModule, args.size() - 2, args.data());

			this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(clingo, &this->gen);

			this->disableWarnings(true);

			// read alica background knowledge from static file
			supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
			string alicaBackGroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("alicaBackgroundKnowledgeFile", NULL);
			alicaBackGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																					alicaBackGroundKnowledgeFile);
			cout << "ASPSolver: " << alicaBackGroundKnowledgeFile << endl;
			this->clingo->load(alicaBackGroundKnowledgeFile);
			this->queryCounter = 0;
#ifdef ASPSolver_DEBUG
			this->modelCount = 0;
#endif
		}

		ASPSolver::~ASPSolver()
		{
			delete this->gringoModule;
		}

		void ASPSolver::load(string filename)
		{
			this->clingo->load(std::forward<string>(filename));
		}

		void ASPSolver::ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context)
		{
			this->clingo->ground(std::forward<Gringo::Control::GroundVec const &>(vec),
									std::forward<Gringo::Any&&>(context));
		}

		vector<Gringo::Value> ASPSolver::createQueryValues(std::string const &query)
		{
			vector<Gringo::Value> ret;
			if (query.find(",") != string::npos && query.find(";") == string::npos)
			{
				size_t start = 0;
				size_t end = string::npos;
				string currentQuery = "";
				while (start != string::npos)
				{
					end = query.find(")", start);
					if (end == string::npos)
					{
						break;
					}
					currentQuery = query.substr(start, end - start + 1);
					currentQuery = supplementary::Configuration::trim(currentQuery);
					ret.push_back(this->gringoModule->parseValue(currentQuery));
					start = query.find(",", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else if (query.find(";") != string::npos)
			{
				size_t start = 0;
				size_t end = string::npos;
				string currentQuery = "";
				while (start != string::npos)
				{
					end = query.find(")", start);
					if (end == string::npos)
					{
						break;
					}
					currentQuery = query.substr(start, end - start + 1);
					currentQuery = supplementary::Configuration::trim(currentQuery);
					ret.push_back(this->gringoModule->parseValue(currentQuery));
					start = query.find(";", end);
					if (start != string::npos)
					{
						start += 1;
					}
				}
			}
			else
			{
				ret.push_back(this->gringoModule->parseValue(query));
			}
			return ret;
		}

		std::vector<Gringo::Value> ASPSolver::getAllMatches(Gringo::Value queryValue)
		{
			std::vector<Gringo::Value> gringoValues;

			this->clingo->solve([queryValue, &gringoValues, this](Gringo::Model const &m)
			{
				//std::cout << "Inside Lambda!" << std::endl;
					ClingoModel& clingoModel = (ClingoModel&) m;
					auto it = clingoModel.out.domains.find(queryValue.sig());

					std::vector<Gringo::AtomState const *> atomStates;
					if (it != clingoModel.out.domains.end())
					{
						//std::cout << "In Loop" << std::endl;
						for (auto& domainPair : it->second.domain)
						{
							//std::cout << domainPair.first << " " << std::endl;
							if (&(domainPair.second) && clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
							{
								if (checkMatchValues(&queryValue, &domainPair.first))
								{
									gringoValues.push_back(domainPair.first);
									atomStates.push_back(&(domainPair.second));
								}
							}
						}
					}

//					for (auto &gringoValue : gringoValues)
//					{
//						std::cout << gringoValue << " ";
//					}
//					std::cout << std::endl;

					return true;
				},
					{});

			return gringoValues;
		}

		bool ASPSolver::registerQuery(shared_ptr<AspQuery> query)
		{
			bool ret = true;
			auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
			if (entry == this->registeredQueries.end())
			{
				this->registeredQueries.push_back(query);
				ret &= true;
			}
			else
			{
				ret &= false;
			}
			return ret;
		}

		bool ASPSolver::unRegisterQuery(shared_ptr<AspQuery> query)
		{
			bool ret = true;
			auto entry = find(this->registeredQueries.begin(), this->registeredQueries.end(), query);
			if (entry != this->registeredQueries.end())
			{
				this->registeredQueries.erase(entry);
				ret &= true;
			}
			else
			{
				ret &= false;
			}
			return ret;
		}

		DefaultGringoModule* ASPSolver::getGringoModule()
		{
			return this->gringoModule;
		}

		const void* ASPSolver::getWildcardPointer()
		{
			return WILDCARD_POINTER;
		}

		const string& ASPSolver::getWildcardString()
		{
			return WILDCARD_STRING;
		}

		bool ASPSolver::solve()
		{
			this->reduceLifeTime();
			this->currentModels.clear();
			auto result = this->clingo->solve(std::bind(&ASPSolver::onModel, this, std::placeholders::_1), {});
			this->removeDeadQueries();
			if (result == Gringo::SolveResult::SAT)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ASPSolver::onModel(const Gringo::Model& m)
		{
#ifdef ASPSolver_DEBUG
			this->modelCount++;
			cout << "ASPSolver: Found the following model which is number " << this->modelCount << ":" << endl;
			for (auto &atom : m.atoms(Gringo::Model::SHOWN))
			{
				std::cout << atom << " ";
			}
			std::cout << std::endl;
#endif

			ClingoModel& clingoModel = (ClingoModel&)m;
			this->currentModels.push_back(m.atoms(Gringo::Model::SHOWN));
			bool foundSomething = false;
			for (auto& query : this->registeredQueries)
			{
				query->getCurrentModels()->push_back(m.atoms(Gringo::Model::SHOWN));
				//	cout << "ASPSolver: processing query '" << queryMapPair.first << "'" << endl;

				// determine the domain of the query predicate
				for (auto queryValue : query->getQueryValues())
				{
					cout << "ASPSolver::onModel: " << queryValue << endl;
					auto it = clingoModel.out.domains.find(queryValue.sig());
					if (it == clingoModel.out.domains.end())
					{
//						cout << "ASPSolver: Didn't find any suitable domain!" << endl;
						continue;
					}

					for (auto& domainPair : it->second.domain)
					{
//							cout << "ASPSolver: Inside domain-loop!" << endl;

						if (&(domainPair.second)
								&& clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
						{
//										cout << "ASPSolver: Found true literal '" << domainPair.first << "'" << endl;

							if (this->checkMatchValues(&queryValue, &domainPair.first))
							{
//										cout << "ASPSolver: Literal '" << domainPair.first << "' matched!" << endl;
								foundSomething = true;
								query->saveStaisfiedPredicate(queryValue, domainPair.first);
								break;
							}
							else
							{
//											cout << "ASPSolver: Literal '" << domainPair.first << "' didn't match!" << endl;

							}
						}
					}
				}
				for (auto rule : query->getRuleModelMap())
				{
					cout << "ASPSolver::onModel: " << rule.first << endl;
					auto it = clingoModel.out.domains.find(rule.first.sig());
					if (it == clingoModel.out.domains.end())
					{
//						cout << "ASPSolver: Didn't find any suitable domain!" << endl;
						continue;
					}

					for (auto& domainPair : it->second.domain)
					{
//							cout << "ASPSolver: Inside domain-loop!" << endl;

						if (&(domainPair.second)
								&& clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
						{
//										cout << "ASPSolver: Found true literal '" << domainPair.first << "'" << endl;

							if (this->checkMatchValues(&rule.first, &domainPair.first))
							{
//										cout << "ASPSolver: Literal '" << domainPair.first << "' matched!" << endl;
								foundSomething = true;
								query->saveRuleModelPair(rule.first, domainPair.first);
								break;
							}
							else
							{
//											cout << "ASPSolver: Literal '" << domainPair.first << "' didn't match!" << endl;

							}
						}
					}
				}
			}
			return foundSomething;
		}

		bool ASPSolver::checkMatchValues(const Gringo::Value* value1, const Gringo::Value* value2)
		{
			if (value2->type() != Gringo::Value::Type::FUNC)
				return false;

			if (value1->name() != value2->name())
				return false;

			if (value1->args().size() != value2->args().size())
				return false;

			for (uint i = 0; i < value1->args().size(); ++i)
			{
				Gringo::Value arg = value1->args()[i];

				if (arg.type() == Gringo::Value::Type::ID && arg.name() == ASPSolver::WILDCARD_STRING)
					continue;

				if (arg.type() == Gringo::Value::Type::FUNC && value2->args()[i].type() == Gringo::Value::Type::FUNC)
				{
					if (false == checkMatchValues(&arg, &value2->args()[i]))
						return false;
				}
				else if (arg != value2->args()[i])
					return false;
			}

			return true;
		}

		bool ASPSolver::isTrueForAtLeastOneModel(shared_ptr<AspQuery> query)
		{
			if (query->isDisjunction())
			{
				for (auto queryValue : query->getPredicateModelMap())
				{
					if (queryValue.second.size() == 0)
					{
						return true;
					}
				}
				return false;
			}
			else
			{
				for (auto queryValue : query->getPredicateModelMap())
				{
					if (queryValue.second.size() == 0)
					{
						return false;
					}
				}
			}
			for (auto queryValue : query->getRuleModelMap())
			{
				if (queryValue.second.size() == 0)
				{
					return false;
				}
			}
			return true;
		}

		bool ASPSolver::isTrueForAllModels(shared_ptr<AspQuery> query)
		{
			if (query->isDisjunction())
			{
				for (auto queryValue : query->getPredicateModelMap())
				{
					if (queryValue.second.size() == 0)
					{
						return false;
					}
				}
			}
			else
			{
				for (auto queryValue : query->getPredicateModelMap())
				{
					if (queryValue.second.size() != query->getCurrentModels()->size())
					{
						return false;
					}
				}
			}
			for (auto queryValue : query->getRuleModelMap())
			{
				if (queryValue.second.size() == 0)
				{
					return false;
				}
			}
			return true;
		}

		void ASPSolver::disableWarnings(bool value)
		{
			// TODO implement disableWarnings
//			grOpts_.wNoRedef = value;
//			grOpts_.wNoCycle = value;
//			grOpts_.wNoTermUndef = value;
//			grOpts_.wNoAtomUndef = value;
//			grOpts_.wNoNonMonotone = value;
//			grOpts_.wNoFileIncluded = value;
		}

		bool ASPSolver::existsSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor> >& calls)
		{
			return false;
		}

		bool ASPSolver::getSolution(vector<Variable*>& vars, vector<shared_ptr<ConstraintDescriptor> >& calls,
									vector<void*>& results)
		{
			return false;
		}

		shared_ptr<SolverVariable> ASPSolver::createVariable(long id)
		{
			// TODO necessary for integration into the alica engine
			return make_shared<SolverVariable>();
//			return nullptr; //;
		}

		void ASPSolver::integrateRules()
		{
			for (auto query : this->registeredQueries)
			{
				for (auto rule : query->getRules())
				{
					this->clingo->add("planBase", {}, rule);
				}
			}
			this->clingo->ground( { {"planBase", {}}}, nullptr);
		}

		/**
		 * Validates the well-formedness of a given plan.
		 *
		 * @returns False, if the plan is not valid. True, otherwise.
		 */
		bool ASPSolver::validatePlan(Plan* plan)
		{
			this->planIntegrator->loadPlanTree(plan);
			this->integrateRules();
			this->reduceLifeTime();
			this->currentModels.clear();
			auto result = this->clingo->solve(std::bind(&ASPSolver::onModel, this, std::placeholders::_1), {});
			this->removeDeadQueries();
			if (result == Gringo::SolveResult::SAT)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		void ASPSolver::removeDeadQueries()
		{
			vector<shared_ptr<AspQuery>> toRemove;
			for (auto query : this->registeredQueries)
			{
				if (query->getLifeTime() == 0)
				{
					toRemove.push_back(query);
				}
			}
			for (auto query : toRemove)
			{
				this->unRegisterQuery(query);
			}
		}

		vector<shared_ptr<AspQuery> > ASPSolver::getRegisteredQueries()
		{
			return this->registeredQueries;
		}

		shared_ptr<ClingoLib> ASPSolver::getClingo()
		{
			return this->clingo;
		}

		void ASPSolver::reduceLifeTime()
		{
			for (auto query : this->registeredQueries)
			{
				if (query->getLifeTime() > 0)
				{
					query->reduceLifeTime();
				}
			}
		}

		const long long ASPSolver::getSolvingTime()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			// time in seconds
			return claspFacade->summary().solveTime * 1000;
		}

		const long long ASPSolver::getSatTime()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			// time in seconds
			return claspFacade->summary().satTime * 1000;
		}

		const long long ASPSolver::getUnsatTime()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			// time in seconds
			return claspFacade->summary().unsatTime * 1000;
		}

		const long ASPSolver::getModelCount()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			return claspFacade->summary().enumerated();
		}

		int ASPSolver::getQueryCounter()
		{
			this->queryCounter++;
			return this->queryCounter;
		}
		const long ASPSolver::getAtomCount()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			return claspFacade->summary().lpStats()->atoms;
		}

		const long ASPSolver::getBodiesCount()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			return claspFacade->summary().lpStats()->bodies;
		}

		const long ASPSolver::getAuxAtomsCount()
		{
			auto claspFacade = this->clingo->clasp;

			if (claspFacade == nullptr)
				return -1;

			return claspFacade->summary().lpStats()->auxAtoms;
		}

		void ASPSolver::printStats()
		{
			auto claspFacade = this->clingo->clasp;

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

	} /* namespace reasoner */
} /* namespace alica */


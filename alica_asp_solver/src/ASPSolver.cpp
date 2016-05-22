/*
 * ASPSolver.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPSolver.h"
#include "engine/model/Plan.h"

namespace alica
{
	namespace reasoner
	{

		const void* const ASPSolver::WILDCARD_POINTER = new int(0);
		const string ASPSolver::WILDCARD_STRING = "wildcard";

		ASPSolver::ASPSolver(AlicaEngine* ae, std::vector<char const*> args) :
				IConstraintSolver(ae), gen(ASPSolver::WILDCARD_POINTER, ASPSolver::WILDCARD_STRING)
		{
			this->clingo = make_shared<ClingoLib>(gringoModule, args.size() - 2, args.data());

			this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(clingo, &this->gen);

			this->disableWarnings(true);

			// read alica background knowledge from static file
			supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
			string alicaBackGroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("alicaBackgroundKnowledgeFile", NULL);
			alicaBackGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																					alicaBackGroundKnowledgeFile);
			cout << "ASPSolver: " << alicaBackGroundKnowledgeFile << endl;
			this->clingo->load(alicaBackGroundKnowledgeFile);
		}

		ASPSolver::~ASPSolver()
		{

		}

		void ASPSolver::load(string filename)
		{
			this->clingo->load(std::forward<string>(filename));
		}

		void ASPSolver::ground(Gringo::Control::GroundVec const &vec, Gringo::Any &&context)
		{
			cout << "ASPSolver: grounding " << endl;
			this->clingo->ground(std::forward<Gringo::Control::GroundVec const &>(vec),
									std::forward<Gringo::Any&&>(context));
		}

		Gringo::Value ASPSolver::createQueryValue(std::string const &queryString)
		{
			return this->gringoModule.parseValue(queryString);
		}

		bool ASPSolver::isTrue(Gringo::Value queryValue)
		{
			auto entry = this->registeredQueries.find(queryValue);
			if (entry != this->registeredQueries.end() && entry->second.size() > 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ASPSolver::isTrue(const string& queryValue)
		{
			return this->isTrue(this->gringoModule.parseValue(queryValue));
		}

		bool ASPSolver::registerQuery(const string& query)
		{
			Gringo::Value queryValue = this->gringoModule.parseValue(query);
			auto entry = this->registeredQueries.find(queryValue);
			if (entry == this->registeredQueries.end())
			{
				this->registeredQueries.emplace(queryValue, vector<Gringo::Value>());
				return true;
			}
			else
			{
				return false;
			}
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
			return nullptr; //make_shared<SolverVariable>();
		}

		/**
		 * Validates the well-formedness of a given plan.
		 *
		 * @returns False, if the plan is not valid. True, otherwise.
		 */
		bool ASPSolver::validatePlan(Plan* plan)
		{
			this->planIntegrator->loadPlanTree(plan);

			auto result = this->clingo->solve(std::bind(&ASPSolver::onModel, this, std::placeholders::_1), {});
			if (result == Gringo::SolveResult::SAT)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ASPSolver::onModel(Gringo::Model const &m)
		{
#ifdef ASPSolver_DEBUG
			cout << "ASPSolver: Found the following model:" << endl;
			for (auto &atom : m.atoms(Gringo::Model::SHOWN))
			{
				std::cout << atom << " ";
			}
			std::cout << std::endl;
#endif

			ClingoModel& clingoModel = (ClingoModel&)m;
			bool foundSomething = false;
			for (auto& queryMapPair : this->registeredQueries)
			{
				//	cout << "ASPSolver: processing query '" << queryMapPair.first << "'" << endl;

				queryMapPair.second.clear();
				//std::vector<Gringo::AtomState const *> atomStates;

				// determine the domain of the query predicate
				cout << "ASPSolver::onModel: " << queryMapPair.first << endl;
				auto it = clingoModel.out.domains.find(queryMapPair.first.sig());
				if (it == clingoModel.out.domains.end())
				{
					cout << "ASPSolver: Didn't find any suitable domain!" << endl;
					continue;
				}

				for (auto& domainPair : it->second.domain)
				{
					//	cout << "ASPSolver: Inside domain-loop!" << endl;

					if (&(domainPair.second)
							&& clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
					{
						//			cout << "ASPSolver: Found true literal '" << domainPair.first << "'" << endl;

						if (this->checkMatchValues(&queryMapPair.first, &domainPair.first))
						{
							//			cout << "ASPSolver: Literal '" << domainPair.first << "' matched!" << endl;
							foundSomething = true;
							queryMapPair.second.push_back(domainPair.first);
							//atomStates.push_back(&(domainPair.second));
						}
						else
						{
							//				cout << "ASPSolver: Literal '" << domainPair.first << "' didn't match!" << endl;

						}
					}
				}
			}

			return foundSomething;
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
			ss << "TOTAL Time: " << claspFacade->summary().totalTime<< endl;
			ss << "CPU Time: " << claspFacade->summary().cpuTime<< endl;
			ss << "SAT Time: " << (claspFacade->summary().satTime*1000.0) << endl;
			ss << "UNSAT Time: " << (claspFacade->summary().unsatTime*1000.0)<< endl;
			ss << "SOLVE Time: " << (claspFacade->summary().solveTime*1000.0)<< endl;

			cout << ss.str() << flush;
		}

	} /* namespace reasoner */
} /* namespace alica */


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

		ASPSolver::ASPSolver(AlicaEngine* ae) :
				IConstraintSolver(ae)
		{
			std::vector<char const *> args {"clingo", "-e", "brave", nullptr};

			this->clingo = make_shared<ClingoLib>(gringoModule, args.size() - 2, args.data());

			this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(clingo);

			this->disableWarnings(true);

			// read alica background knowledge from static file
			supplementary::SystemConfig* sc = supplementary::SystemConfig::getInstance();
			string alicaBackGroundKnowledgeFile = (*sc)["ASPSolver"]->get<string>("alicaBackgroundKnowledgeFile", NULL);
			alicaBackGroundKnowledgeFile = supplementary::FileSystem::combinePaths((*sc).getConfigPath(),
																					alicaBackGroundKnowledgeFile);
			cout << "ASPSolver: " << alicaBackGroundKnowledgeFile << endl;
			this->clingo->load(alicaBackGroundKnowledgeFile);

			this->clingo->ground({ {"PlanBase", {}}}, nullptr);

			Gringo::Value queryGringoValue = this->gringoModule.parseValue("plan(wildcard)");

			this->clingo->solve([queryGringoValue, this](Gringo::Model const &m)
			{
				//std::cout << "Inside Lambda!" << std::endl;
					ClingoModel& clingoModel = (ClingoModel&) m;
					auto it = clingoModel.out.domains.find(queryGringoValue.sig());
					std::vector<Gringo::Value> gringoValues;
					std::vector<Gringo::AtomState const *> atomStates;
					if (it != clingoModel.out.domains.end())
					{
						//std::cout << "In Loop" << std::endl;
						for (auto& domainPair : it->second.domain)
						{
							//std::cout << domainPair.first << " " << std::endl;
							if (&(domainPair.second) && clingoModel.model->isTrue(clingoModel.lp.getLiteral(domainPair.second.uid())))
							{
								if (checkMatchValues(&queryGringoValue, &domainPair.first))
								{
									gringoValues.push_back(domainPair.first);
									atomStates.push_back(&(domainPair.second));
								}
							}
						}
					}

					for (auto &gringoValue : gringoValues)
					{
						std::cout << gringoValue << " ";
					}
					std::cout << std::endl;

					return true;
				},
					{});
		}

		ASPSolver::~ASPSolver()
		{
			// TODO Auto-generated destructor stub
		}

		void ASPSolver::load(string filename)
		{
			this->clingo->load(std::forward<string>(filename));
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

				if (arg.type() == Gringo::Value::Type::ID && arg.name() == "wildcard")
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
			// TODO
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
			// TODO
			return nullptr; //make_shared<SolverVariable>();
		}

		/**
		 * Validates the well-formedness of a given plan.
		 */
		bool ASPSolver::validatePlan(Plan* plan)
		{

			return this->planIntegrator->loadPlanTree(plan);

			// Ask for broken stuff related to the given plan

			// Present results
		}

	} /* namespace reasoner */
} /* namespace alica */

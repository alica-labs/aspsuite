/*
 * ASPAlicaPlanIntegrator.cpp
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPAlicaPlanIntegrator.h"

#include "engine/model/Plan.h"
#include "engine/model/PlanType.h"
#include "engine/model/BehaviourConfiguration.h"
#include "engine/model/State.h"
#include "engine/model/FailureState.h"
#include "engine/model/SuccessState.h"
#include "engine/model/Task.h"
#include "engine/model/EntryPoint.h"

using namespace std;

namespace alica
{
	namespace reasoner
	{

		ASPAlicaPlanIntegrator::ASPAlicaPlanIntegrator(shared_ptr<ClingoLib> clingo)
		{
			this->clingo = clingo;
		}

		ASPAlicaPlanIntegrator::~ASPAlicaPlanIntegrator()
		{
		}

		bool ASPAlicaPlanIntegrator::loadPlanTree(Plan* p)
		{
			this->processedPlanIds.clear();

			// Add master plan as topLevelPlan
			this->clingo->add("PlanBase", {}, "topLevelPlan(p" + std::to_string(p->getId()) + ").");

			// Start recursive integration of plan tree
			bool hasTreeProperty = this->processPlan(p);

			// Ground the created plan base
			this->clingo->ground( { {"PlanBase", {}}}, nullptr);

			// TODO: This should only be done on queries, so its currently here for easier testing.
			this->clingo->solve(nullptr, {});

			return hasTreeProperty;
		}

		/**
		 * Recursively integrates the given plan into the data-structures of the asp solver.
		 * @param Plan p.
		 * @return Returns true, if the processed plan is a tree.
		 */
		bool ASPAlicaPlanIntegrator::processPlan(Plan* p)
		{
			long currentPlanId = p->getId();
			if (find(processedPlanIds.begin(), processedPlanIds.end(), currentPlanId) != processedPlanIds.end())
			{// already processed, so there is a cycle
				cout << "PlanIntegrator: Cycle detected!" << endl;
				return false;
			}
			else
			{
				processedPlanIds.push_back(currentPlanId);
			}

			// flag to signal, if the plan tree really is a tree
			bool hasTreeProperty = true;

			cout << "ASPAlicaPlanIntegrator: processing plan " << p->getName() << " (ID: " << p->getId() << ")" << endl;

			// add the plan
			this->clingo->add("PlanBase", {}, "plan(p" + std::to_string(p->getId()) + ").");

			// add entry point and their tasks
			// TODO: Optimize string creation
			for (auto& idEntryPointPair : p->getEntryPoints())
			{
				EntryPoint* entryPoint = idEntryPointPair.second;
				Task* task = entryPoint->getTask();

				// add task
				this->clingo->add("PlanBase", {}, "task(tsk" + std::to_string(task->getId()) + ").");
				this->clingo->add("PlanBase", {}, "hasTask(p" + std::to_string(p->getId())
															  + ", tsk" + std::to_string(task->getId()) + ").");

				// add entry point
				this->clingo->add("PlanBase", {}, "entryPoint(ep" + std::to_string(entryPoint->getId()) + ").");
				this->clingo->add("PlanBase", {}, "hasMinCardinality(ep" + std::to_string(entryPoint->getId()) + ", "
								  	  	  	  	  	  	  	  	  	  	 + entryPoint->getMinCardinality() + ").");
				this->clingo->add("PlanBase", {}, "hasMaxCardinality(ep" + std::to_string(entryPoint->getId()) + ", "
												  	  	  	  	  	  	 + entryPoint->getMaxCardinality() + ").");
				// TODO: success required flag of entry point (for better modeling support)
				this->clingo->add("PlanBase", {}, "hasEntryPoint(p" + std::to_string(p->getId())
																	+ ", tsk" + std::to_string(task->getId())
																	+ ", ep" + std::to_string(entryPoint->getId()) + ").");
			}

			// add state
			for (auto& state : p->getStates())
			{

				this->clingo->add("PlanBase", {}, "hasState(p" + std::to_string(p->getId())
															   + ", s" + std::to_string(state->getId()) + ").");

				if (state->failureState)
				{
					// add failure state
					this->clingo->add("PlanBase", {}, "failureState(s" + std::to_string(state->getId()) + ").");
					// TODO: handle post-condition of failure state
					// ((FailureState) state).getPostCondition();
				}
				else if (state->successState)
				{
					// add success state
					this->clingo->add("PlanBase", {}, "successState(s" + std::to_string(state->getId()) + ").");
					// TODO: handle post-condition of success state
					// ((SuccessState) state).getPostCondition();
				}
				else // normal state
				{
					// add state
					this->clingo->add("PlanBase", {}, "state(s" + std::to_string(state->getId()) + ").");

					for (auto abstractChildPlan : state->getPlans())
					{
						if (alica::Plan* childPlan = dynamic_cast<alica::Plan*>(abstractChildPlan))
						{
							this->clingo->add("PlanBase", {}, "hasPlan(s" + std::to_string(state->getId())
																		  + ", p" + std::to_string(childPlan->getId()) + ").");

							if (!this->processPlan(childPlan))
							{
								// TODO: This way to determine the tree property is not correct! Plans can be arbitrarily reused.
								cout << "ASPAlicaPlanIntegrator: treeProperty violated by plan '"
										<< childPlan->getName() << "'" << endl;
								hasTreeProperty = false;
							}
						}
						else if (alica::PlanType* childPlanType = dynamic_cast<alica::PlanType*>(abstractChildPlan))
						{
							this->clingo->add("PlanBase", {}, "planType(pt" +  std::to_string(childPlanType->getId()) + ").");
							this->clingo->add("PlanBase", {}, "hasPlanType(s" + std::to_string(state->getId())
																			  + ", pt" + std::to_string(childPlanType->getId()) + ").");

							for (auto& childPlan : childPlanType->getPlans())
							{
								this->clingo->add("PlanBase", {}, "hasRealisation(pt" + std::to_string(childPlanType->getId())
																					  + ", p" + std::to_string(childPlan->getId()) + ").");
								if (!this->processPlan(childPlan))
								{
									// TODO: This way to determine the tree property is not correct! Plans can be arbitrarily reused.
									cout << "ASPAlicaPlanIntegrator: treeProperty violated by plan '"
											<< childPlan->getName() << "'" << endl;
									hasTreeProperty = false;
								}
							}
						}
						else if (alica::BehaviourConfiguration* childBehaviourConf = dynamic_cast<alica::BehaviourConfiguration*>(abstractChildPlan))
						{
							// TODO: Handle BehaviourConfigurations
						}
					}
				}
			}

			return hasTreeProperty;
		}

	} /* namespace reasoner */
} /* namespace alica */

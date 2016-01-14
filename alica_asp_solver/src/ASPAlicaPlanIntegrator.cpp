/*
 * ASPAlicaPlanIntegrator.cpp
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPAlicaPlanIntegrator.h"
#include "alica_asp_solver/ASPGenerator.h"


#include <engine/model/Plan.h>
#include <engine/model/PlanType.h>
#include <engine/model/BehaviourConfiguration.h>
#include <engine/model/State.h>
#include <engine/model/FailureState.h>
#include <engine/model/SuccessState.h>
#include <engine/model/Task.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/SyncTransition.h>

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
			this->clingo->add("PlanBase", {}, gen.plan(p));

			// add entry points and their tasks
			for (auto& idEntryPointPair : p->getEntryPoints())
			{
				EntryPoint* entryPoint = idEntryPointPair.second;
				Task* task = entryPoint->getTask();

				// add task
				// TODO: what is, if we add the task two or more times to PlanBase?
				// TODO: should I also integrate the IDLE Task ?
				this->clingo->add("PlanBase", {}, gen.task(task));
				this->clingo->add("PlanBase", {}, gen.hasTask(p, task));

				// add entry point
				this->clingo->add("PlanBase", {}, gen.entryPoint(entryPoint));
				this->clingo->add("PlanBase", {}, gen.successRequired(entryPoint));
				this->clingo->add("PlanBase", {}, gen.hasInitialState(entryPoint, entryPoint->getState()));
				this->clingo->add("PlanBase", {}, gen.hasMinCardinality(entryPoint, entryPoint->getMinCardinality()));
				this->clingo->add("PlanBase", {}, gen.hasMaxCardinality(entryPoint, entryPoint->getMaxCardinality()));
				this->clingo->add("PlanBase", {}, gen.hasEntryPoint(p, task, entryPoint));
				this->clingo->add("PlanBase", {}, gen.hasEntryPoint(p, task, entryPoint));
			}

			// add state
			for (auto& state : p->getStates())
			{

				this->clingo->add("PlanBase", {}, gen.hasState(p, state));

				if (state->isFailureState())
				{
					// add failure state
					this->clingo->add("PlanBase", {}, gen.failureState(state));

					// TODO: handle post-condition of failure state
					// ((FailureState) state).getPostCondition();
				}
				else if (state->isSuccessState())
				{
					// add success state
					this->clingo->add("PlanBase", {}, gen.successState(state));

					// TODO: handle post-condition of success state
					// ((SuccessState) state).getPostCondition();
				}
				else // normal state
				{
					// add state
					this->clingo->add("PlanBase", {}, gen.state(state));

					for (auto& abstractChildPlan : state->getPlans())
					{
						if (alica::Plan* childPlan = dynamic_cast<alica::Plan*>(abstractChildPlan))
						{
							this->clingo->add("PlanBase", {}, gen.hasPlan(state, childPlan));

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
							this->clingo->add("PlanBase", {}, gen.planType(childPlanType));
							this->clingo->add("PlanBase", {}, gen.hasPlanType(state, childPlanType));

							for (auto& childPlan : childPlanType->getPlans())
							{
								this->clingo->add("PlanBase", {}, gen.hasRealisation(childPlanType, childPlan));
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

				// add state transition relations
				for (auto& inTransition : state->getInTransitions())
				{
					this->clingo->add("PlanBase", {}, gen.hasInTransition(state, inTransition));
				}
				for (auto& outTransition : state->getOutTransitions())
				{
					this->clingo->add("PlanBase", {}, gen.hasOutTransition(state, outTransition));
				}
			}

			// add transitions
			for (auto& transition : p->getTransitions())
			{
				this->clingo->add("PlanBase", {}, gen.transition(transition));

				//TODO: handle pre-conditions of transitions
			}

			// add synchronisations
			for (auto& syncTransition : p->getSyncTransitions())
			{
				this->clingo->add("PlanBase", {}, gen.syncTransition(syncTransition));
				for (auto& transition : syncTransition->getInSync())
				{
					this->clingo->add("PlanBase", {}, gen.hasSynchedTransition(syncTransition, transition));
				}
				// TODO: maybe it is nice to have the timeouts of a sync transition
			}

			return hasTreeProperty;
		}

	} /* namespace reasoner */
} /* namespace alica */

/*
 * ASPAlicaPlanIntegrator.cpp
 *
 *  Created on: Sep 9, 2015
 *      Author: Stephan Opfer
 */

#include "asp_solver_wrapper/ASPAlicaPlanIntegrator.h"
#include "asp_solver_wrapper/ASPGenerator.h"

#include <engine/model/Plan.h>
#include <engine/model/PlanType.h>
#include <engine/model/BehaviourConfiguration.h>
#include <engine/model/State.h>
#include <engine/model/FailureState.h>
#include <engine/model/SuccessState.h>
#include <engine/model/Task.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/SyncTransition.h>
#include <engine/model/RuntimeCondition.h>
#include <engine/model/PreCondition.h>
#include <engine/model/Condition.h>

#include <CustomHashes.h>
#include <regex>

using namespace std;

namespace alica
{
	namespace reasoner
	{

		ASPAlicaPlanIntegrator::ASPAlicaPlanIntegrator(::reasoner::IASPSolver* solver, ASPGenerator* gen)
		{
			this->solver = solver;
			this->gen = gen;
		}

		ASPAlicaPlanIntegrator::~ASPAlicaPlanIntegrator()
		{
		}

		void ASPAlicaPlanIntegrator::loadPlanTree(Plan* p)
		{
			this->processedPlanIds.clear();

			/* Creates running plan instance of root plan and must
			 * be called before processPlan(p), in order to have the
			 * instanceElementHash correctly calculated for all child
			 * plans.
			 */
			uint64_t instanceElementHash = this->handleRunningPlan(p);

			// Start recursive integration of plan tree
			this->processPlan(p, instanceElementHash);

			// Ground the created plan base
			this->solver->ground( { {"planBase", {}}}, nullptr);
			this->solver->ground( { {"alicaBackground", {}}}, nullptr);
		}

		/**
		 * Recursively integrates the given plan into the data-structures of the asp solver.
		 * @param Plan p.
		 * @return Returns true, if the processed plan is a tree.
		 */
		void ASPAlicaPlanIntegrator::processPlan(Plan* p, uint64_t instanceElementHash)
		{
			long currentPlanId = p->getId();
			if (find(processedPlanIds.begin(), processedPlanIds.end(), currentPlanId) != processedPlanIds.end())
			{ // already processed
				return;
			}

			processedPlanIds.push_back(currentPlanId);

#ifdef ASPAlicaPlanIntegratorDebug
			cout << "ASPAlicaPlanIntegrator: processing plan " << p->getName() << " (ID: " << p->getId() << ")" << endl;
#endif
			// add the plan
			this->solver->add("planBase", {}, gen->plan(p));

			// TODO: add pre- and run-time condition of plan
			this->processPreCondition(p->getPreCondition());
			this->processRuntimeCondition(p->getRuntimeCondition());

			// add entry points and their tasks
			for (auto& idEntryPointPair : p->getEntryPoints())
			{
				EntryPoint* entryPoint = idEntryPointPair.second;
				Task* task = entryPoint->getTask();

				// add task
				// TODO: what is, if we add the task two or more times to planBase?
				// TODO: should I also integrate the IDLE Task ?
				// - for runtime evaluation this should be part of the ALICA background knowledge
				this->solver->add("planBase", {}, gen->task(task));
				this->solver->add("planBase", {}, gen->hasTask(p, task));

				// add entry point
				this->solver->add("planBase", {}, gen->entryPoint(entryPoint));
				if (entryPoint->isSuccessRequired())
				{
					this->solver->add("planBase", {}, gen->successRequired(entryPoint));
				}
				this->solver->add("planBase", {}, gen->hasInitialState(entryPoint, entryPoint->getState()));
				this->solver->add("planBase", {}, gen->hasMinCardinality(entryPoint, entryPoint->getMinCardinality()));
				this->solver->add("planBase", {}, gen->hasMaxCardinality(entryPoint, entryPoint->getMaxCardinality()));
				this->solver->add("planBase", {}, gen->hasEntryPoint(p, task, entryPoint));
				this->solver->add("planBase", {}, gen->hasEntryPoint(p, task, entryPoint));
			}

			// add state
			for (auto& state : p->getStates())
			{

				this->solver->add("planBase", {}, gen->hasState(p, state));

				if (state->isFailureState())
				{
					// add failure state
					this->solver->add("planBase", {}, gen->failureState(state));

					// TODO: handle post-condition of failure state
					// ((FailureState) state).getPostCondition();
				}
				else if (state->isSuccessState())
				{
					// add success state
					this->solver->add("planBase", {}, gen->successState(state));

					// TODO: handle post-condition of success state
					// ((SuccessState) state).getPostCondition();
				}
				else // normal state
				{
					// add state
					this->solver->add("planBase", {}, gen->state(state));

					for (auto& abstractChildPlan : state->getPlans())
					{
						if (alica::Plan* childPlan = dynamic_cast<alica::Plan*>(abstractChildPlan))
						{
							this->solver->add("planBase", {}, gen->hasPlan(state, childPlan));

							instanceElementHash = handleRunningPlan(childPlan, state, instanceElementHash);

							this->processPlan(childPlan, instanceElementHash);
						}
						else if (alica::PlanType* childPlanType = dynamic_cast<alica::PlanType*>(abstractChildPlan))
						{
							this->solver->add("planBase", {}, gen->planType(childPlanType));
							this->solver->add("planBase", {}, gen->hasPlanType(state, childPlanType));

							for (auto& childPlan : childPlanType->getPlans())
							{
								this->solver->add("planBase", {}, gen->hasRealisation(childPlanType, childPlan));

								instanceElementHash = handleRunningPlan(childPlan, state, childPlanType, instanceElementHash);

								this->processPlan(childPlan, instanceElementHash);
							}
						}
						else if (alica::BehaviourConfiguration* childBehaviourConf =
								dynamic_cast<alica::BehaviourConfiguration*>(abstractChildPlan))
						{
							// TODO: Handle BehaviourConfigurations
							this->solver->add("planBase", {}, gen->behaviourConf(childBehaviourConf));
							this->solver->add("planBase", {}, gen->hasBehaviourConf(state, childBehaviourConf));
							// TODO behaviour necessary ?
							// TODO handle pre- and runtime condition of beh conf
						}
					}
				}

				// add state transition relations
				for (auto& inTransition : state->getInTransitions())
				{
					this->solver->add("planBase", {}, gen->hasInTransition(state, inTransition));
				}
				for (auto& outTransition : state->getOutTransitions())
				{
					this->solver->add("planBase", {}, gen->hasOutTransition(state, outTransition));
				}
			}

			// add transitions
			for (auto& transition : p->getTransitions())
			{
				this->solver->add("planBase", {}, gen->transition(transition));

				//TODO: handle pre-conditions of transitions
			}

			// add synchronisations
			for (auto& syncTransition : p->getSyncTransitions())
			{
				this->solver->add("planBase", {}, gen->synchronisation(syncTransition));
				for (auto& transition : syncTransition->getInSync())
				{
					this->solver->add("planBase", {}, gen->hasSynchedTransition(syncTransition, transition));
				}
				// TODO: maybe it is nice to have the timeouts of a sync transition
			}
		}

		void ASPAlicaPlanIntegrator::processPreCondition(PreCondition* cond)
		{
			if (!cond || !cond->isEnabled())
			{
				return;
			}

			// alica program facts
			this->solver->add("planBase", {}, gen->preCondition(cond));
			if (Plan* plan = dynamic_cast<Plan*>(cond->getAbstractPlan()))
			{
				this->solver->add("planBase", {}, gen->hasPreCondition(plan, cond));
			}

			// asp encoded precondition
			if (cond->getConditionString() != "")
			{
				const string& condString = cond->getConditionString();

				cout << "ASP-Integrator: " << gen->preConditionHolds(cond) << endl;
				this->solver->add("planBase", {}, gen->preConditionHolds(cond));

				// analysis of asp encoded precondition, because of non-local in relations
				handleCondString(condString, "preCond", cond);
			}
		}

		void ASPAlicaPlanIntegrator::processRuntimeCondition(RuntimeCondition* cond)
		{
			//TODO finish RuntimeCondition
			if (!cond)
			{
				return;
			}

			// alica program facts
			this->solver->add("planBase", {}, gen->runtimeCondition(cond));
			if (Plan* plan = dynamic_cast<Plan*>(cond->getAbstractPlan()))
			{
				this->solver->add("planBase", {}, gen->hasRuntimeCondition(plan, cond));
			}

			if (cond->getConditionString() != "")
			{
				const string& condString = cond->getConditionString();

				cout << "ASP-Integrator: " << gen->runtimeConditionHolds(cond) << endl;
				this->solver->add("planBase", {}, gen->runtimeConditionHolds(cond));

				// analysis of asp encoded precondition, because of non-local in relations
				handleCondString(condString, "runtimeCond", cond);
			}

		}

		uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(Plan* rootPlan)
		{
			uint64_t instanceElementHash = supplementary::CustomHashes::FNV_OFFSET
						^ rootPlan->getId() * supplementary::CustomHashes::FNV_MAGIC_PRIME;
			this->solver->add("planBase", {}, gen->hasPlanInstance(rootPlan, instanceElementHash));
			this->solver->add("planBase", {}, gen->runningPlan(instanceElementHash));
			return instanceElementHash;
		}


		uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(Plan* plan, State* state, uint64_t instanceElementHash)
		{
			instanceElementHash = instanceElementHash ^ state->getId() * supplementary::CustomHashes::FNV_MAGIC_PRIME;
			this->solver->add("planBase", {}, gen->hasRunningPlan(state, instanceElementHash));
			this->solver->add("planBase", {}, gen->hasPlanInstance(plan, instanceElementHash));
			this->solver->add("planBase", {}, gen->runningPlan(instanceElementHash));
			return instanceElementHash;
		}

		uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(Plan* plan, State* state, PlanType* planType, uint64_t instanceElementHash)
		{
			instanceElementHash = instanceElementHash ^ planType->getId() * supplementary::CustomHashes::FNV_MAGIC_PRIME;
			instanceElementHash = handleRunningPlan(plan, state, instanceElementHash);
			this->solver->add("planBase", {}, gen->hasRunningRealisation(planType, instanceElementHash));
			return instanceElementHash;
		}


		void ASPAlicaPlanIntegrator::handleCondString(const string& condString, string prefix, Condition* cond)
		{
			// analysis of asp encoded precondition, because of non-local in relations
			std::regex words_regex(
					"((\\s|,){1}|^)in\\((\\s*)([A-Z]+(\\w*))(\\s*),(\\s*)([a-z]+(\\w*))(\\s*)(,(\\s*)([a-zA-Z]+(\\w*))(\\s*)){2}\\)");
			auto words_begin = std::sregex_iterator(condString.begin(), condString.end(), words_regex);
			auto words_end = std::sregex_iterator();
			for (std::sregex_iterator i = words_begin; i != words_end; ++i)
			{
				std::smatch match = *i;
				std::string inPredicateString = match.str();
//				std::cout << "ASPAlicaPlanInegrator: ALL MATCH>>>>>>" << inPredicateString << "<<<<<<" << std::endl;
				;
				size_t start = inPredicateString.find(',');
				size_t end = string::npos;
				string plan = "";
				string task = "";
				string state = "";
				// plan
				if (start != string::npos)
				{
					end = inPredicateString.find(',', start + 1);
					if (end != string::npos)
					{
						plan = inPredicateString.substr(start + 1, end - start - 1);
						this->solver->add("planBase", {}, gen->inRefPlan(prefix, cond, plan));
//						std::cout << "ASPAlicaPlanInegrator: PLAN MATCH>>>>>>" << plan << "<<<<<<" << std::endl;
					}
				}
				// task
				start = end + 1;
				if (start != string::npos)
				{
					end = inPredicateString.find(',', start);
					if (end != string::npos)
					{
						task = inPredicateString.substr(start, end - start);
						task = supplementary::Configuration::trim(task);
//						std::cout << "ASPAlicaPlanInegrator: TASK MATCH>>>>>>" << task << "<<<<<<" << std::endl;
						if (islower(task.at(0)))
						{
							this->solver->add("planBase", {}, gen->inRefPlanTask(prefix, cond, plan, task));
						}
						else
						{
							task = "";
						}
					}
				}
				// state
				start = end + 1;
				if (start != string::npos)
				{
					end = inPredicateString.find(')', start);
					if (end != string::npos)
					{
						state = inPredicateString.substr(start, end - start);
						state = supplementary::Configuration::trim(state);
//						std::cout << "ASPAlicaPlanInegrator: STATE MATCH>>>>>>" << state << "<<<<<<" << std::endl;
						if (islower(state.at(0)))
						{
							this->solver->add("planBase", {}, gen->inRefPlanState(prefix, cond, plan, state));
						}
						else
						{
							state = "";
						}
					}
				}
				// plan, task, state
				if (task != "" && state != "")
				{
					this->solver->add("planBase", {}, gen->inRefPlanTaskState(prefix, cond, plan, task, state));
				}
			}
		}

	} /* namespace reasoner */
} /* namespace alica */

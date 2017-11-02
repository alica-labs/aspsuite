/*
 * ASPGenerator.h
 *
 *  Created on: Jan 11, 2016
 *      Author: Stephan Opfer
 */

#ifndef SRC_ASPSOLVERWRAPPERGENERATOR_H_
#define SRC_ASPSOLVERWRAPPERGENERATOR_H_

#include <string>

#include <engine/model/Plan.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/State.h>
#include <engine/model/Task.h>
#include <engine/model/PlanType.h>
#include <engine/model/Transition.h>
#include <engine/model/SyncTransition.h>
#include <engine/model/Condition.h>
#include <engine/model/PreCondition.h>
#include <engine/model/RuntimeCondition.h>
#include <engine/model/BehaviourConfiguration.h>

namespace alica
{
	namespace reasoner
	{
		class ASPGenerator
		{
		public:
			ASPGenerator(const void* wildcard_pointer, string wildcard_string);

			// UNARY PREDICATES
			string plan(Plan* p, bool dotTerminated = true);
			string entryPoint(EntryPoint* ep, bool dotTerminated = true);
			string state(State* s, bool dotTerminated = true);
			string task(Task* t, bool dotTerminated = true);
			string failureState(State* s, bool dotTerminated = true);
			string successState(State* s, bool dotTerminated = true);
			string planType(PlanType* s, bool dotTerminated = true);
			string successRequired(EntryPoint* ep, bool dotTerminated = true);
			string transition(Transition* t, bool dotTerminated = true);
			string synchronisation(SyncTransition* sync, bool dotTerminated = true);
			string preCondition(PreCondition* cond, bool dotTerminated = true);
			string runtimeCondition(RuntimeCondition* cond, bool dotTerminated = true);
			string runningPlan(uint64_t instanceElementHash, bool dotTerminated = true);
			string behaviourConf(BehaviourConfiguration* behConf, bool dotTerminated = true);

			string brokenPlan(Plan* s, bool dotTerminated = true);
			string cyclic(Plan* p, bool dotTerminated = true);
			string cycleFree(Plan* p, bool dotTerminated = true);
			string brokenState(State* s, bool dotTerminated = true);
			string brokenEntryPoint(EntryPoint* e, bool dotTerminated = true);
			string brokenSynchronisation(SyncTransition* sync, bool dotTerminated = true);
			string neglocal(PreCondition* cond, bool dotTerminated = true);
			string brokenRunningPlan(uint64_t instanceElementHash, bool dotTerminated = true);


			// BINARY PREDICATES
			string hasTask(Plan* p, Task* t, bool dotTerminated = true);
			string hasMinCardinality(EntryPoint* ep, int minCard, bool dotTerminated = true);
			string hasMaxCardinality(EntryPoint* ep, int maxCard, bool dotTerminated = true);
			string hasState(Plan* p, State* s, bool dotTerminated = true);
			string hasInitialState(EntryPoint* ep, State* s, bool dotTerminated = true);
			string hasPlan(State* s, Plan* p, bool dotTerminated = true);
			string hasPlanType(State* s, PlanType* pt, bool dotTerminated = true);
			string hasRealisation(PlanType* pt, Plan* p, bool dotTerminated = true);
			string hasPreCondition(Plan* p, PreCondition* cond, bool dotTerminated = true);
			string hasRuntimeCondition(Plan* p, RuntimeCondition* cond, bool dotTerminated = true);
			string hasInTransition(State* s, Transition* t, bool dotTerminated = true);
			string hasOutTransition(State* s, Transition* t, bool dotTerminated = true);
			string hasSynchedTransition(SyncTransition* sync, Transition* t, bool dotTerminated = true);
			string brokenPlanTaskPair(Plan* p, Task*t, bool dotTerminated = true);
			string inRefPlan(string prefix, Condition* c, string plan, bool dotTerminated = true);
			string hasPlanInstance(Plan* p, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasRunningPlan(State* s, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasRunningRealisation(PlanType* pt, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasBehaviourConf(State* s, BehaviourConfiguration* behConf, bool dotTerminated = true);

			// TERNARY PREDICATES
			string hasEntryPoint(Plan* p, Task* t, EntryPoint* ep, bool dotTerminated = true);
			string inRefPlanTask(string prefix, Condition* c, string plan, string task, bool dotTerminated = true);
			string inRefPlanState(string prefix, Condition* c, string plan, string state, bool dotTerminated = true);

			// QUATERNARY PREDICATES
			string inRefPlanTaskState(string prefix, Condition* c, string plan, string task, string state, bool dotTerminated = true);

			// RULES
			string preConditionHolds(PreCondition* cond);
			string runtimeConditionHolds(RuntimeCondition* cond);


			string get(Plan* p);
			string get(EntryPoint* ep);
			string get(State* s);
			string get(Task* t);
			string get(PlanType* pt);
			string get(Transition* t);
			string get(SyncTransition* sync);
			string get(string prefix, Condition* cond);
			string get(string prefix, uint64_t instanceElementHash);
			string get(BehaviourConfiguration* behConf);

		private:
			// maps from id to asp string for all ALICA elements
			std::map<long, string> elements;
			// maps from id to asp string for all instance elements
			std::map<long, string> instanceElements;

			const void* wildcard_pointer;
			string wildcard_string;
		};
	}
}
#endif /* SRC_ASPSOLVERWRAPPERGENERATOR_H_ */



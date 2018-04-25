#pragma once

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
#include <engine/model/Behaviour.h>

namespace alica
{
	namespace reasoner
	{
		class ASPGenerator
		{
		public:
			ASPGenerator(const void* wildcard_pointer, string wildcard_string);

			// UNARY PREDICATES
			string plan(const Plan* p, bool dotTerminated = true);
			string entryPoint(const EntryPoint* ep, bool dotTerminated = true);
			string state(const State* s, bool dotTerminated = true);
			string task(const Task* t, bool dotTerminated = true);
			string failureState(const State* s, bool dotTerminated = true);
			string successState(const State* s, bool dotTerminated = true);
			string planType(const PlanType* s, bool dotTerminated = true);
			string successRequired(const EntryPoint* ep, bool dotTerminated = true);
			string transition(const Transition* t, bool dotTerminated = true);
			string synchronisation(const SyncTransition* sync, bool dotTerminated = true);
			string preCondition(const PreCondition* cond, bool dotTerminated = true);
			string runtimeCondition(const RuntimeCondition* cond, bool dotTerminated = true);
			string runningPlan(uint64_t instanceElementHash, bool dotTerminated = true);
			string behaviour(const Behaviour* beh, bool dotTerminated = true);

			string brokenPlan(const Plan* s, bool dotTerminated = true);
			string cyclic(const Plan* p, bool dotTerminated = true);
			string cycleFree(const Plan* p, bool dotTerminated = true);
			string brokenState(const State* s, bool dotTerminated = true);
			string brokenEntryPoint(const EntryPoint* e, bool dotTerminated = true);
			string brokenSynchronisation(const SyncTransition* sync, bool dotTerminated = true);
			string neglocal(const PreCondition* cond, bool dotTerminated = true);
			string brokenRunningPlan(uint64_t instanceElementHash, bool dotTerminated = true);


			// BINARY PREDICATES
			string hasTask(const Plan* p, const Task* t, bool dotTerminated = true);
			string hasMinCardinality(const EntryPoint* ep, int minCard, bool dotTerminated = true);
			string hasMaxCardinality(const EntryPoint* ep, int maxCard, bool dotTerminated = true);
			string hasState(const Plan* p, const State* s, bool dotTerminated = true);
			string hasInitialState(const EntryPoint* ep, const State* s, bool dotTerminated = true);
			string hasPlan(const State* s, const Plan* p, bool dotTerminated = true);
			string hasPlanType(const State* s, const PlanType* pt, bool dotTerminated = true);
			string hasRealisation(const PlanType* pt, const Plan* p, bool dotTerminated = true);
			string hasPreCondition(const Plan* p, const PreCondition* cond, bool dotTerminated = true);
			string hasRuntimeCondition(const Plan* p, const RuntimeCondition* cond, bool dotTerminated = true);
			string hasInTransition(const State* s, const Transition* t, bool dotTerminated = true);
			string hasOutTransition(const State* s, const Transition* t, bool dotTerminated = true);
			string hasSynchedTransition(const SyncTransition* sync, const Transition* t, bool dotTerminated = true);
			string brokenPlanTaskPair(const Plan* p, const Task*t, bool dotTerminated = true);
			string inRefPlan(string prefix, const Condition* c, string plan, bool dotTerminated = true);
			string hasPlanInstance(const Plan* p, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasRunningPlan(const State* s, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasRunningRealisation(const PlanType* pt, uint64_t instanceElementHash, bool dotTerminated = true);
			string hasBehaviour(const State* s, const Behaviour* behConf, bool dotTerminated = true);

			// TERNARY PREDICATES
			string hasEntryPoint(const Plan* p, const Task* t, const EntryPoint* ep, bool dotTerminated = true);
			string inRefPlanTask(string prefix, const Condition* c, string plan, string task, bool dotTerminated = true);
			string inRefPlanState(string prefix, const Condition* c, string plan, string state, bool dotTerminated = true);

			// QUATERNARY PREDICATES
			string inRefPlanTaskState(string prefix, const Condition* c, string plan, string task, string state, bool dotTerminated = true);

			// RULES
			string preConditionHolds(const PreCondition* cond);
			string runtimeConditionHolds(const RuntimeCondition* cond);


			string get(const Plan* p);
			string get(const EntryPoint* ep);
			string get(const State* s);
			string get(const Task* t);
			string get(const PlanType* pt);
			string get(const Transition* t);
			string get(const SyncTransition* sync);
			string get(string prefix, const Condition* cond);
			string get(string prefix, uint64_t instanceElementHash);
			string get(const Behaviour* beh);

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



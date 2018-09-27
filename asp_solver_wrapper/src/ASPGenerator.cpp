#include "asp_solver_wrapper/ASPGenerator.h"

namespace alica
{
	namespace reasoner
	{
		ASPGenerator::ASPGenerator(const void* wildcard_pointer, std::string wildcard_string) :
				wildcard_pointer(wildcard_pointer), wildcard_string(std::move(wildcard_string))
		{
		}

		// UNARY Predicates

		std::string ASPGenerator::plan(const Plan* p, bool dotTerminated)
		{
			return "plan(" + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::entryPoint(const EntryPoint* ep, bool dotTerminated)
		{
			return "entryPoint(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::state(const State* s, bool dotTerminated)
		{
			return "state(" + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::task(const Task* t, bool dotTerminated)
		{
			return "task(" + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::failureState(const State* s, bool dotTerminated)
		{
			return "failureState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::successState(const State* s, bool dotTerminated)
		{
			return "successState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::planType(const PlanType* pt, bool dotTerminated)
		{
			return "planType(" + get(pt) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::successRequired(const EntryPoint* ep, bool dotTerminated)
		{
			return "successRequired(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::transition(const Transition* t, bool dotTerminated)
		{
			return "transition(" + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::synchronisation(const SyncTransition* sync, bool dotTerminated)
		{
			return "synchronisation(" + get(sync) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::preCondition(const PreCondition* cond, bool dotTerminated)
		{
			return "preCondition(" + get("preCond", cond) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenPlan(const Plan* p, bool dotTerminated)
		{
			return "brokenPlan(" + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::cyclic(const Plan* p, bool dotTerminated)
		{
			return "cyclic(" + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::cycleFree(const Plan* p, bool dotTerminated)
		{
			return "cycleFree(" + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenState(const State* s, bool dotTerminated)
		{
			return "brokenState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenEntryPoint(const EntryPoint* ep, bool dotTerminated)
		{
			return "brokenEntryPoint(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenSynchronisation(const SyncTransition* sync, bool dotTerminated)
		{
			return "brokenSynchronisation(" + get(sync) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::neglocal(const PreCondition* cond, bool dotTerminated)
		{
			return "nonlocal(" + get("preCond", cond) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::runningPlan(uint64_t instanceElementHash, bool dotTerminated)
		{
			return "runningPlan(" + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::runtimeCondition(const RuntimeCondition* cond, bool dotTerminated)
		{
			return "runtimeCondition(" + get("runtimeCond", cond) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenRunningPlan(uint64_t instanceElementHash, bool dotTerminated)
		{
			return "brokenRunningPlan(" + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::behaviour(const Behaviour* beh, bool dotTerminated)
		{
			return "behaviourConf(" + get(beh) + (dotTerminated ? ")." : ")");
		}

		// BINARY Predicates

		std::string ASPGenerator::hasTask(const Plan* p, const Task* t, bool dotTerminated)
		{
			return "hasTask(" + get(p) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasMinCardinality(const EntryPoint* ep, int minCard, bool dotTerminated)
		{
			return "hasMinCardinality(" + get(ep) + ", " + std::to_string(minCard) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasMaxCardinality(const EntryPoint* ep, int maxCard, bool dotTerminated)
		{
			return "hasMaxCardinality(" + get(ep) + ", " + std::to_string(maxCard) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasState(const Plan* p, const State* s, bool dotTerminated)
		{
			return "hasState(" + get(p) + ", " + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasInitialState(const EntryPoint* ep, const State* s, bool dotTerminated)
		{
			return "hasInitialState(" + get(ep) + ", " + get(s) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasPlan(const State* s, const Plan* p, bool dotTerminated)
		{
			return "hasPlan(" + get(s) + ", " + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasPlanType(const State* s, const PlanType* pt, bool dotTerminated)
		{
			return "hasPlanType(" + get(s) + ", " + get(pt) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasRealisation(const PlanType* pt, const Plan* p, bool dotTerminated)
		{
			return "hasRealisation(" + get(pt) + ", " + get(p) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasPreCondition(const Plan* p, const PreCondition* cond, bool dotTerminated)
		{
			return "hasPreCondition(" + get(p) + ", " + get("preCond", cond) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasInTransition(const State* s, const Transition* t, bool dotTerminated)
		{
			return "hasInTransition(" + get(s) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasOutTransition(const State* s, const Transition* t, bool dotTerminated)
		{
			return "hasOutTransition(" + get(s) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasSynchedTransition(const SyncTransition* sync, const Transition* t, bool dotTerminated)
		{
			return "hasSynchedTransition(" + get(sync) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::brokenPlanTaskPair(const Plan* p, const Task* t, bool dotTerminated)
		{
			return "brokenPlanTaskPair(" + get(p) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::inRefPlan(std::string prefix, const Condition* c, std::string plan, bool dotTerminated)
		{
			return "inRefPlan(" + get(std::move(prefix), c) + ", " + plan + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasPlanInstance(const Plan* p, uint64_t instanceElementHash, bool dotTerminated)
		{
			return "hasPlanInstance(" + get(p) + ", " + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasRuntimeCondition(const Plan* p, const RuntimeCondition* cond, bool dotTerminated)
		{
			return "hasRuntimeCondition(" + get(p) + ", " + get("runtimeCond", cond) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasRunningPlan(const State* s, uint64_t instanceElementHash, bool dotTerminated)
		{
			return "hasRunningPlan(" + get(s) + ", " + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasRunningRealisation(const PlanType* pt, uint64_t instanceElementHash, bool dotTerminated)
		{
			return "hasRunningRealisation(" + get(pt) + ", " + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::hasBehaviour(const State* s, const Behaviour* beh, bool dotTerminated)
		{
			return "hasBehaviour(" + get(s) + ", " + get(beh) + (dotTerminated ? ")." : ")");
		}

		// TERNARY Predicates

		std::string ASPGenerator::hasEntryPoint(const Plan* p, const Task* t, const EntryPoint* ep, bool dotTerminated)
		{
			return "hasEntryPoint(" + get(p) + ", " + get(t) + ", " + get(ep) + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::inRefPlanTask(std::string prefix, const Condition* c, std::string plan, std::string task, bool dotTerminated)
		{
			return "inRefPlanTask(" + get(std::move(prefix), c) + ", " + plan + ", " + task + (dotTerminated ? ")." : ")");
		}

		std::string ASPGenerator::inRefPlanState(std::string prefix, const Condition* c, std::string plan, std::string state, bool dotTerminated)
		{
			return "inRefPlanState(" + get(std::move(prefix), c) + ", " + plan + ", " + state + (dotTerminated ? ")." : ")");
		}

		// QUATERNARY Predicates

		std::string ASPGenerator::inRefPlanTaskState(std::string prefix, const Condition* c, std::string plan, std::string task, std::string state,
												bool dotTerminated)
		{
			return "inRefPlanTaskState(" + get(std::move(prefix), c) + ", " + plan + ", " + task + ", " + state
					+ (dotTerminated ? ")." : ")");
		}

		// RULES

		std::string ASPGenerator::preConditionHolds(const PreCondition* cond)
		{
			return "holds(" + get("preCond", cond) + ") :- " + cond->getConditionString();
		}

		std::string ASPGenerator::runtimeConditionHolds(const RuntimeCondition* cond)
		{
			return "holds(" + get("runtimeCond", cond) + ") :- " + cond->getConditionString();
		}

		// LITERALS

		std::string ASPGenerator::get(const Plan* p)
		{
			if (p == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(p->getId());
			if (iter == this->elements.end())
			{
				return this->elements[p->getId()] = ("p" + std::to_string(p->getId()));
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const EntryPoint* ep)
		{
			if (ep == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(ep->getId());
			if (iter == this->elements.end())
			{
				return this->elements[ep->getId()] = "ep" + std::to_string(ep->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const State* s)
		{
			if (s == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(s->getId());
			if (iter == this->elements.end())
			{
				return this->elements[s->getId()] = "s" + std::to_string(s->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const Task* t)
		{
			if (t == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(t->getId());
			if (iter == this->elements.end())
			{
				return this->elements[t->getId()] = "tsk" + std::to_string(t->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const PlanType* pt)
		{
			if (pt == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(pt->getId());
			if (iter == this->elements.end())
			{
				return this->elements[pt->getId()] = "pt" + std::to_string(pt->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const Transition* t)
		{
			if (t == this->wildcard_pointer)
				return this->wildcard_string;
			auto&& iter = this->elements.find(t->getId());
			if (iter == this->elements.end())
			{
				return this->elements[t->getId()] = "t" + std::to_string(t->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const SyncTransition* sync)
		{
			if (sync == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(sync->getId());
			if (iter == this->elements.end())
			{
				return this->elements[sync->getId()] = "sync" + std::to_string(sync->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(std::string prefix, uint64_t instanceElementHash)
		{
			if (prefix.empty())
				return this->wildcard_string;

			auto&& iter = this->instanceElements.find(instanceElementHash);
			if (iter == this->instanceElements.end())
			{
				return this->instanceElements[instanceElementHash] = prefix + std::to_string(instanceElementHash);
			}
			return iter->second;
		}

		std::string ASPGenerator::get(std::string prefix, const Condition* cond)
		{
			if (cond == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(cond->getId());
			if (iter == this->elements.end())
			{
				return this->elements[cond->getId()] = prefix + std::to_string(cond->getId());
			}
			return iter->second;
		}

		std::string ASPGenerator::get(const Behaviour* beh)
		{
			if (beh == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(beh->getId());
			if (iter == this->elements.end())
			{
				return this->elements[beh->getId()] = "behaviour" + std::to_string(beh->getId());
			}
			return iter->second;
		}

	}
}

/*
 * ASPGenerator.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: Stephan Opfer
 */

#include "alica_asp_solver/ASPGenerator.h"

namespace alica
{
	namespace reasoner
	{
		ASPGenerator::ASPGenerator(const void* wildcard_pointer, string wildcard_string) :
				wildcard_pointer(wildcard_pointer), wildcard_string(wildcard_string)
		{
		}

		// UNARY Predicates

		string ASPGenerator::plan(Plan* p, bool dotTerminated)
		{
			return "plan(" + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::entryPoint(EntryPoint* ep, bool dotTerminated)
		{
			return "entryPoint(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::state(State* s, bool dotTerminated)
		{
			return "state(" + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::task(Task* t, bool dotTerminated)
		{
			return "task(" + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::failureState(State* s, bool dotTerminated)
		{
			return "failureState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::successState(State* s, bool dotTerminated)
		{
			return "successState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::planType(PlanType* pt, bool dotTerminated)
		{
			return "planType(" + get(pt) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::successRequired(EntryPoint* ep, bool dotTerminated)
		{
			return "successRequired(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::transition(Transition* t, bool dotTerminated)
		{
			return "transition(" + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::synchronisation(SyncTransition* sync, bool dotTerminated)
		{
			return "synchronisation(" + get(sync) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::preCondition(PreCondition* cond, bool dotTerminated)
		{
			return "preCondition(" + get(cond) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenPlan(Plan* p, bool dotTerminated)
		{
			return "brokenPlan(" + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::cyclic(Plan* p, bool dotTerminated)
		{
			return "cyclic(" + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::cycleFree(Plan* p, bool dotTerminated)
		{
			return "cycleFree(" + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenState(State* s, bool dotTerminated)
		{
			return "brokenState(" + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenEntryPoint(EntryPoint* ep, bool dotTerminated)
		{
			return "brokenEntryPoint(" + get(ep) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenSynchronisation(SyncTransition* sync, bool dotTerminated)
		{
			return "brokenSynchronisation(" + get(sync) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::neglocal(PreCondition* cond, bool dotTerminated)
		{
			return "nonlocal(" + get(cond) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::runningPlan(uint64_t instanceElementHash, bool dotTerminated)
		{
			return "runningPlan(" + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::runtimeCondition(RuntimeCondition* cond, bool dotTerminated)
		{
			return "runtimeCondition(" + get(cond) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenRunningPlan(uint64_t instanceElementHash, bool dotTerminated)
		{
			return "brokenRunningPlan(" + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::behaviourConf(BehaviourConfiguration* behConf, bool dotTerminated)
		{
			return "behaviourConf(" + get(behConf) + (dotTerminated ? ")." : ")");
		}



		// BINARY Predicates

		string ASPGenerator::hasTask(Plan* p, Task* t, bool dotTerminated)
		{
			return "hasTask(" + get(p) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasMinCardinality(EntryPoint* ep, int minCard, bool dotTerminated)
		{
			return "hasMinCardinality(" + get(ep) + ", " + std::to_string(minCard) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasMaxCardinality(EntryPoint* ep, int maxCard, bool dotTerminated)
		{
			return "hasMaxCardinality(" + get(ep) + ", " + std::to_string(maxCard) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasState(Plan* p, State* s, bool dotTerminated)
		{
			return "hasState(" + get(p) + ", " + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasInitialState(EntryPoint* ep, State* s, bool dotTerminated)
		{
			return "hasInitialState(" + get(ep) + ", " + get(s) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasPlan(State* s, Plan* p, bool dotTerminated)
		{
			return "hasPlan(" + get(s) + ", " + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasPlanType(State* s, PlanType* pt, bool dotTerminated)
		{
			return "hasPlanType(" + get(s) + ", " + get(pt) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasRealisation(PlanType* pt, Plan* p, bool dotTerminated)
		{
			return "hasRealisation(" + get(pt) + ", " + get(p) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasPreCondition(Plan* p, PreCondition* cond, bool dotTerminated)
		{
			return "hasPreCondition(" + get(p) + ", " + get(cond) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasInTransition(State* s, Transition* t, bool dotTerminated)
		{
			return "hasInTransition(" + get(s) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasOutTransition(State* s, Transition* t, bool dotTerminated)
		{
			return "hasOutTransition(" + get(s) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasSynchedTransition(SyncTransition* sync, Transition* t, bool dotTerminated)
		{
			return "hasSynchedTransition(" + get(sync) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::brokenPlanTaskPair(Plan* p, Task* t, bool dotTerminated)
		{
			return "brokenPlanTaskPair(" + get(p) + ", " + get(t) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlan(PreCondition* c, string plan, bool dotTerminated)
		{
			return "inRefPlan(" + get(c) + ", " + plan + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlan(RuntimeCondition* c, string plan, bool dotTerminated)
		{
			return "inRefPlan(" + get(c) + ", " + plan + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasPlanInstance(Plan* p, uint64_t instanceElementHash, bool dotTerminated)
		{
			return "hasPlanInstance(" + get(p) + ", " + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasRuntimeCondition(Plan* p, RuntimeCondition* cond, bool dotTerminated)
		{
			return "hasRuntimeCondition(" + get(p) + ", " + get(cond) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasRunningPlan(State* s, uint64_t instanceElementHash, bool dotTerminated)
		{
			return "hasRunningPlan(" + get(s) + ", " + get("rp", instanceElementHash) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::hasBehaviourConf(State* s, BehaviourConfiguration* behConf, bool dotTerminated)
		{
			return "hasBehaviourConf(" + get(s) + ", " + get(behConf) + (dotTerminated ? ")." : ")");
		}

		// TERNARY Predicates

		string ASPGenerator::hasEntryPoint(Plan* p, Task* t, EntryPoint* ep, bool dotTerminated)
		{
			return "hasEntryPoint(" + get(p) + ", " + get(t) + ", " + get(ep) + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlanTask(PreCondition* c, string plan, string task, bool dotTerminated)
		{
			return "inRefPlanTask(" + get(c) + ", " + plan + ", " + task + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlanState(PreCondition* c, string plan, string state, bool dotTerminated)
		{
			return "inRefPlanState(" + get(c) + ", " + plan + ", " + state + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlanTask(RuntimeCondition* c, string plan, string task, bool dotTerminated)
		{
			return "inRefPlanTask(" + get(c) + ", " + plan + ", " + task + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlanState(RuntimeCondition* c, string plan, string state, bool dotTerminated)
		{
			return "inRefPlanState(" + get(c) + ", " + plan + ", " + state + (dotTerminated ? ")." : ")");
		}

		// QUATERNARY Predicates

		string ASPGenerator::inRefPlanTaskState(PreCondition* c, string plan, string task, string state, bool dotTerminated)
		{
			return "inRefPlanTaskState(" + get(c) + ", " + plan +  ", " + task + ", " + state + (dotTerminated ? ")." : ")");
		}

		string ASPGenerator::inRefPlanTaskState(RuntimeCondition* c, string plan, string task, string state,
																			bool dotTerminated)
		{
			return "inRefPlanTaskState(" + get(c) + ", " + plan +  ", " + task + ", " + state + (dotTerminated ? ")." : ")");
		}

		// RULES

		string ASPGenerator::preConditionHolds(PreCondition* cond)
		{
			return "holds(" + get(cond) + ") :- " + cond->getConditionString();
		}


		string ASPGenerator::runtimeConditionHolds(RuntimeCondition* cond)
		{
			return "holds(" + get(cond) + ") :- " + cond->getConditionString();
		}

		// LITERALS

		string ASPGenerator::get(Plan* p)
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

		string ASPGenerator::get(EntryPoint* ep)
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

		string ASPGenerator::get(State* s)
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

		string ASPGenerator::get(Task* t)
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

		string ASPGenerator::get(PlanType* pt)
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

		string ASPGenerator::get(Transition* t)
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

		string ASPGenerator::get(SyncTransition* sync)
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

		string ASPGenerator::get(PreCondition* cond)
		{
			if (cond == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(cond->getId());
			if (iter == this->elements.end())
			{
				return this->elements[cond->getId()] = "preCond" + std::to_string(cond->getId());
			}
			return iter->second;
		}

		string ASPGenerator::get(RuntimeCondition* cond)
		{
			if (cond == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(cond->getId());
			if (iter == this->elements.end())
			{
				return this->elements[cond->getId()] = "runtimeCond" + std::to_string(cond->getId());
			}
			return iter->second;
		}

		string ASPGenerator::get(string prefix, uint64_t instanceElementHash)
		{
			if (prefix == "")
				return this->wildcard_string;

			auto&& iter = this->instanceElements.find(instanceElementHash);
			if (iter == this->instanceElements.end())
			{
				return this->instanceElements[instanceElementHash] = prefix + std::to_string(instanceElementHash);
			}
			return iter->second;
		}

		string ASPGenerator::get(BehaviourConfiguration* behConf)
		{
			if (behConf == this->wildcard_pointer)
				return this->wildcard_string;

			auto&& iter = this->elements.find(behConf->getId());
			if (iter == this->elements.end())
			{
				return this->elements[behConf->getId()] = "behaviourConf" + std::to_string(behConf->getId());
			}
			return iter->second;
		}

	}
}

#pragma once

#include <string>

#include <engine/model/Behaviour.h>
#include <engine/model/Condition.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/Plan.h>
#include <engine/model/PlanType.h>
#include <engine/model/PreCondition.h>
#include <engine/model/RuntimeCondition.h>
#include <engine/model/State.h>
#include <engine/model/Synchronisation.h>
#include <engine/model/Task.h>
#include <engine/model/Transition.h>

namespace alica
{
namespace reasoner
{
class ASPGenerator
{
public:
    ASPGenerator(const void* wildcard_pointer, std::string wildcard_string);

    // UNARY PREDICATES
    std::string plan(const Plan* p, bool dotTerminated = true);
    std::string entryPoint(const EntryPoint* ep, bool dotTerminated = true);
    std::string state(const State* s, bool dotTerminated = true);
    std::string task(const Task* t, bool dotTerminated = true);
    std::string failureState(const State* s, bool dotTerminated = true);
    std::string successState(const State* s, bool dotTerminated = true);
    std::string planType(const PlanType* s, bool dotTerminated = true);
    std::string successRequired(const EntryPoint* ep, bool dotTerminated = true);
    std::string transition(const Transition* t, bool dotTerminated = true);
    std::string synchronisation(const Synchronisation* sync, bool dotTerminated = true);
    std::string preCondition(const PreCondition* cond, bool dotTerminated = true);
    std::string runtimeCondition(const RuntimeCondition* cond, bool dotTerminated = true);
    std::string runningPlan(uint64_t instanceElementHash, bool dotTerminated = true);
    std::string behaviour(const Behaviour* beh, bool dotTerminated = true);

    std::string brokenPlan(const Plan* s, bool dotTerminated = true);
    std::string cyclic(const Plan* p, bool dotTerminated = true);
    std::string cycleFree(const Plan* p, bool dotTerminated = true);
    std::string brokenState(const State* s, bool dotTerminated = true);
    std::string brokenEntryPoint(const EntryPoint* e, bool dotTerminated = true);
    std::string brokenSynchronisation(const Synchronisation* sync, bool dotTerminated = true);
    std::string neglocal(const PreCondition* cond, bool dotTerminated = true);
    std::string brokenRunningPlan(uint64_t instanceElementHash, bool dotTerminated = true);

    // BINARY PREDICATES
    std::string hasTask(const Plan* p, const Task* t, bool dotTerminated = true);
    std::string hasMinCardinality(const EntryPoint* ep, int minCard, bool dotTerminated = true);
    std::string hasMaxCardinality(const EntryPoint* ep, int maxCard, bool dotTerminated = true);
    std::string hasState(const Plan* p, const State* s, bool dotTerminated = true);
    std::string hasInitialState(const EntryPoint* ep, const State* s, bool dotTerminated = true);
    std::string hasPlan(const State* s, const Plan* p, bool dotTerminated = true);
    std::string hasPlanType(const State* s, const PlanType* pt, bool dotTerminated = true);
    std::string hasRealisation(const PlanType* pt, const Plan* p, bool dotTerminated = true);
    std::string hasPreCondition(const Plan* p, const PreCondition* cond, bool dotTerminated = true);
    std::string hasRuntimeCondition(const Plan* p, const RuntimeCondition* cond, bool dotTerminated = true);
    std::string hasInTransition(const State* s, const Transition* t, bool dotTerminated = true);
    std::string hasOutTransition(const State* s, const Transition* t, bool dotTerminated = true);
    std::string hasSynchedTransition(const Synchronisation* sync, const Transition* t, bool dotTerminated = true);
    std::string brokenPlanTaskPair(const Plan* p, const Task* t, bool dotTerminated = true);
    std::string inRefPlan(std::string prefix, const Condition* c, std::string plan, bool dotTerminated = true);
    std::string hasPlanInstance(const Plan* p, uint64_t instanceElementHash, bool dotTerminated = true);
    std::string hasRunningPlan(const State* s, uint64_t instanceElementHash, bool dotTerminated = true);
    std::string hasRunningRealisation(const PlanType* pt, uint64_t instanceElementHash, bool dotTerminated = true);
    std::string hasBehaviour(const State* s, const Behaviour* behConf, bool dotTerminated = true);

    // TERNARY PREDICATES
    std::string hasEntryPoint(const Plan* p, const Task* t, const EntryPoint* ep, bool dotTerminated = true);
    std::string inRefPlanTask(std::string prefix, const Condition* c, std::string plan, std::string task, bool dotTerminated = true);
    std::string inRefPlanState(std::string prefix, const Condition* c, std::string plan, std::string state, bool dotTerminated = true);

    // QUATERNARY PREDICATES
    std::string inRefPlanTaskState(std::string prefix, const Condition* c, std::string plan, std::string task, std::string state, bool dotTerminated = true);

    // RULES
    std::string preConditionHolds(const PreCondition* cond);
    std::string runtimeConditionHolds(const RuntimeCondition* cond);

    std::string get(const Plan* p);
    std::string get(const EntryPoint* ep);
    std::string get(const State* s);
    std::string get(const Task* t);
    std::string get(const PlanType* pt);
    std::string get(const Transition* t);
    std::string get(const Synchronisation* sync);
    std::string get(std::string prefix, const Condition* cond);
    std::string get(std::string prefix, uint64_t instanceElementHash);
    std::string get(const Behaviour* beh);

private:
    // maps from id to asp std::string for all ALICA elements
    std::map<long, std::string> elements;
    // maps from id to asp std::string for all instance elements
    std::map<long, std::string> instanceElements;

    const void* wildcard_pointer;
    std::string wildcard_string;
};
} // namespace reasoner
} // namespace alica

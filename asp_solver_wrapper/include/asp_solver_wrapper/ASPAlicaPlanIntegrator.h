#pragma once

#include <asp_commons/IASPSolver.h>
#include <memory>

//#define ASPAlicaPlanIntegratorDebug

namespace alica
{
class Plan;
class PreCondition;
class RuntimeCondition;
class Condition;
class State;
class PlanType;

namespace reasoner
{

class ASPGenerator;

//! Integrates ALICA program into ASP.
/**
 * Helps to integrate all parts of an ALCIA program into the ASP solver.
 */
class ASPAlicaPlanIntegrator
{
  public:
    ASPAlicaPlanIntegrator(::reasoner::IASPSolver *solver, ASPGenerator *gen);
    virtual ~ASPAlicaPlanIntegrator();
    void loadPlanTree(const Plan *p);

  private:
    void processPlan(const Plan *p, uint64_t instanceElementHash);
    void processPreCondition(const PreCondition *cond);
    void processRuntimeCondition(const RuntimeCondition *cond);
    uint64_t handleRunningPlan(const Plan *rootPlan);
    uint64_t handleRunningPlan(const Plan *childPlan, const State *state, uint64_t instanceElementHash);
    uint64_t handleRunningPlan(const Plan *childPlan, const State *state, const PlanType *planType, uint64_t instanceElementHash);
    void handleCondString(const std::string &condString, std::string prefix, const Condition *cond);

    ASPGenerator *gen;
    ::reasoner::IASPSolver *solver;

    std::vector<long> processedPlanIds;
};

} /* namespace reasoner */
} /* namespace alica */

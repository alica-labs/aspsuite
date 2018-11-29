#pragma once

#include <asp_commons/AnnotatedValVec.h>
#include <engine/constraintmodul/ISolver.h>
#include <engine/model/Variable.h>

#include <memory>
#include <string>
#include <vector>

namespace reasoner
{
class IASPSolver;
class ASPCommonsTerm;
class ASPCommonsVariable;
class ASPQuery;
} // namespace reasoner

namespace alica
{
class SolverVariable;
class SolverTerm;
class AlicaEngine;
class Plan;
namespace reasoner
{
class ASPAlicaPlanIntegrator;
class ASPGenerator;
class ASPSolverWrapper : public alica::ISolver<ASPSolverWrapper, ::reasoner::AnnotatedValVec*>
{
public:
    ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args);
    virtual ~ASPSolverWrapper();

    bool existsSolutionImpl(SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls);
    bool getSolutionImpl(SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls, std::vector<::reasoner::AnnotatedValVec*>& results);

    virtual SolverVariable* createVariable(int64_t representingVariableID, SolverContext* ctx) override;
    virtual std::unique_ptr<SolverContext> createSolverContext() override;

    ::reasoner::IASPSolver* getSolver();
    void init(::reasoner::IASPSolver* solver);
    int getQueryCounter();
    bool validatePlan(Plan* plan);
    alica::reasoner::ASPGenerator* gen;

    void removeDeadQueries();
    bool registerQuery(std::shared_ptr<::reasoner::ASPQuery> query);
    bool unregisterQuery(std::shared_ptr<::reasoner::ASPQuery> query);
    void printStats();
    std::vector<std::shared_ptr<::reasoner::ASPQuery>> getRegisteredQueries();

private:
    AlicaEngine* ae;
    ::reasoner::IASPSolver* solver;
    bool masterPlanLoaded;
    std::shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
    ::reasoner::ASPCommonsTerm* toCommonsTerm(SolverTerm* term);
};
} // namespace reasoner

} /* namespace alica */

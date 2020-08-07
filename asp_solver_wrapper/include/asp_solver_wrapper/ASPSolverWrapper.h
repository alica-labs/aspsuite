#pragma once

#include <engine/constraintmodul/ISolver.h>
#include <engine/model/Variable.h>
#include <reasoner/asp/AnnotatedValVec.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace reasoner
{
namespace asp
{
class Solver;
class Term;
class AVariable;
class Query;
} // namespace asp
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
class ASPSolverWrapper : public alica::ISolver<ASPSolverWrapper, ::reasoner::asp::AnnotatedValVec*>
{
public:
    ASPSolverWrapper(alica::AlicaEngine* ae);
    virtual ~ASPSolverWrapper();

    bool existsSolutionImpl(SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls);
    bool getSolutionImpl(
            SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls, std::vector<::reasoner::asp::AnnotatedValVec*>& results);

    virtual SolverVariable* createVariable(int64_t representingVariableID, SolverContext* ctx) override;
    virtual std::unique_ptr<SolverContext> createSolverContext() override;

    ::reasoner::asp::Solver* getSolver();
    void init(::reasoner::asp::Solver* solver);
    void reset();
    int generateQueryID();
    bool validatePlan(Plan* plan);
    alica::reasoner::ASPGenerator* gen;

    void removeDeadQueries();
    void registerQuery(std::shared_ptr<::reasoner::asp::Query> query);
    void unregisterQuery(std::shared_ptr<::reasoner::asp::Query> query);
    void printStats();
    const std::unordered_map<int, std::shared_ptr<::reasoner::asp::Query>> getRegisteredQueries() const;
    bool resetted;

private:
    AlicaEngine* ae;
    ::reasoner::asp::Solver* solver;
    bool masterPlanLoaded;
    std::shared_ptr<ASPAlicaPlanIntegrator> planIntegrator;
    ::reasoner::asp::Term* toCommonsTerm(SolverTerm* term);
};
} // namespace reasoner

} /* namespace alica */

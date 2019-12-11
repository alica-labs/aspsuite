#include "asp_solver_wrapper/ASPSolverWrapper.h"

#include <asp_solver_wrapper/ASPTerm.h>
#include <asp_solver_wrapper/ASPVariable.h>
#include <engine/AlicaEngine.h>
#include <engine/PlanBase.h>
#include <engine/constraintmodul/ProblemDescriptor.h>

#include <reasoner/asp/Term.h>
#include <reasoner/asp/Variable.h>
#include <reasoner/asp/Query.h>
#include <reasoner/asp/AnnotatedValVec.h>
#include <reasoner/asp/Solver.h>
#include <asp_solver_wrapper/ASPAlicaPlanIntegrator.h>
#include <asp_solver_wrapper/ASPGenerator.h>
#include <asp_solver_wrapper/ASPSolverContext.h>

namespace alica
{
namespace reasoner
{
ASPSolverWrapper::ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args)
        : alica::ISolver<ASPSolverWrapper, ::reasoner::asp::AnnotatedValVec*>(ae)
{
    this->ae = ae;
    this->solver = nullptr;
    this->planIntegrator = nullptr;
    this->masterPlanLoaded = false;
    this->gen = nullptr;
    this->resetted = false;
}

ASPSolverWrapper::~ASPSolverWrapper()
{
    delete this->solver;
    delete this->gen;
}

int ASPSolverWrapper::generateQueryID()
{
    return this->solver->generateQueryID();
}

bool ASPSolverWrapper::existsSolutionImpl(SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls)
{
    ASPSolverContext* solverCtx = static_cast<ASPSolverContext*>(ctx);

    if (!masterPlanLoaded) {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }

    auto cVars = std::vector<::reasoner::asp::Variable*>(solverCtx->getVariables().size());
    for (unsigned int i = 0; i < solverCtx->getVariables().size(); ++i) {
        cVars.at(i) = (::reasoner::asp::Variable*) solverCtx->getVariables().at(i).get();
    }
    std::vector<::reasoner::asp::Term*> constraint;
    for (auto& c : calls) {
        constraint.push_back(toCommonsTerm(c->getConstraint()));
    }
    bool ret = this->solver->existsSolution(cVars, constraint);
    return ret;
}

bool ASPSolverWrapper::getSolutionImpl(
        SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls, std::vector<::reasoner::asp::AnnotatedValVec*>& results)
{
    ASPSolverContext* solverCtx = static_cast<ASPSolverContext*>(ctx);

    if (!masterPlanLoaded) {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }

    auto cVars = std::vector<::reasoner::asp::Variable*>(solverCtx->getVariables().size());
    for (unsigned int i = 0; i < solverCtx->getVariables().size(); ++i) {
        cVars.at(i) = (::reasoner::asp::Variable*) solverCtx->getVariables().at(i).get();
    }

    std::vector<::reasoner::asp::Term*> constraint;
    for (auto& c : calls) {
        constraint.push_back(toCommonsTerm(c->getConstraint()));
    }
    bool ret = this->solver->getSolution(cVars, constraint, results);
    return ret;
}

SolverVariable* ASPSolverWrapper::createVariable(int64_t representingVariableID, SolverContext* ctx)
{
    return static_cast<ASPSolverContext*>(ctx)->createVariable(representingVariableID);
}

std::unique_ptr<SolverContext> ASPSolverWrapper::createSolverContext()
{
    return std::unique_ptr<SolverContext>(new ASPSolverContext());
}

/**
 * TODO: Why does this method copy the Solver Term?
 * @param term
 * @return
 */
::reasoner::asp::Term* ASPSolverWrapper::toCommonsTerm(SolverTerm* term)
{
    auto tmp = (::reasoner::asp::Term*) term;

    ::reasoner::asp::Term* ret = new ::reasoner::asp::Term();
    ret->setLifeTime(tmp->getLifeTime());
    ret->setQueryRule(tmp->getQueryRule());
    for (auto it : tmp->getRules()) {
        ret->addRule(it);
    }
    for (auto it : tmp->getFacts()) {
        ret->addFact(it);
    }
    ret->setId(tmp->getId());
//    ret->setQueryId(tmp->getQueryId());
    ret->setExternals(tmp->getExternals());
    ret->setNumberOfModels(tmp->getNumberOfModels());
    ret->setProgramSection(tmp->getProgramSection());
    ret->setType(tmp->getType());
    return ret;
}

::reasoner::asp::Solver* ASPSolverWrapper::getSolver()
{
    return solver;
}

void ASPSolverWrapper::init(::reasoner::asp::Solver* solver)
{
    this->solver = solver;
    this->gen = new ASPGenerator(solver->WILDCARD_POINTER, solver->WILDCARD_STRING);
    this->planIntegrator = std::make_shared<ASPAlicaPlanIntegrator>(this->solver, this->gen);
}

void ASPSolverWrapper::reset() {

    //the solver is initially created in the base
    delete this->solver;
    this->solver = new ::reasoner::asp::Solver({});
    delete this->gen;
    this->gen = new ASPGenerator(solver->WILDCARD_POINTER, solver->WILDCARD_STRING);
    this->planIntegrator = std::make_shared<ASPAlicaPlanIntegrator>(this->solver, this->gen);
//    std::cout << "ASPSolverWrapper: Resetted!" << std::endl;
    this->resetted = true;
}

/**
 * Validates the well-formedness of a given plan.
 *
 * @returns False, if the plan is not valid. True, otherwise.
 */
bool ASPSolverWrapper::validatePlan(Plan* plan)
{
    // adds all facts about the given plan tree in to clingo
    this->planIntegrator->loadPlanTree(plan);
    return this->solver->solve();
}

void alica::reasoner::ASPSolverWrapper::removeDeadQueries()
{
    this->solver->removeDeadQueries();
}

void alica::reasoner::ASPSolverWrapper::registerQuery(std::shared_ptr<::reasoner::asp::Query> query)
{
    this->solver->registerQuery(query);
}

void alica::reasoner::ASPSolverWrapper::unregisterQuery(std::shared_ptr<::reasoner::asp::Query> query)
{
    this->solver->unregisterQuery(query);
}

void alica::reasoner::ASPSolverWrapper::printStats()
{
    this->solver->printStats();
}

    std::vector<std::shared_ptr<::reasoner::asp::Query>> alica::reasoner::ASPSolverWrapper::getRegisteredQueries()
{
    return this->solver->getRegisteredQueries();
}
} // namespace reasoner

} /* namespace alica */

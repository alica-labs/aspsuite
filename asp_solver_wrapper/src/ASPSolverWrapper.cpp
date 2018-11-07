#include "asp_solver_wrapper/ASPSolverWrapper.h"

#include <asp_solver_wrapper/ASPTerm.h>
#include <asp_solver_wrapper/ASPVariable.h>
#include <engine/AlicaEngine.h>
#include <engine/PlanBase.h>
#include <engine/constraintmodul/ProblemDescriptor.h>

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPCommonsVariable.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/AnnotatedValVec.h>
#include <asp_commons/IASPSolver.h>
#include <asp_solver_wrapper/ASPAlicaPlanIntegrator.h>
#include <asp_solver_wrapper/ASPGenerator.h>
#include <asp_solver_wrapper/ASPSolverContext.h>

namespace alica
{
namespace reasoner
{
ASPSolverWrapper::ASPSolverWrapper(AlicaEngine* ae, std::vector<char const*> args)
        : alica::ISolver<ASPSolverWrapper, ::reasoner::AnnotatedValVec*>(ae)
{
    this->ae = ae;
    this->solver = nullptr;
    this->planIntegrator = nullptr;
    this->masterPlanLoaded = false;
    this->gen = nullptr;
}

ASPSolverWrapper::~ASPSolverWrapper()
{
    delete this->solver;
    delete this->gen;
}

int ASPSolverWrapper::getQueryCounter()
{
    return this->solver->getQueryCounter();
}

bool ASPSolverWrapper::existsSolutionImpl(SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls)
{
    ASPSolverContext* solverCtx = static_cast<ASPSolverContext*>(ctx);

    if (!masterPlanLoaded) {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }

    auto cVars = std::vector<::reasoner::ASPCommonsVariable*>(solverCtx->getVariables().size());
    for (unsigned int i = 0; i < solverCtx->getVariables().size(); ++i) {
        cVars.at(i) = (::reasoner::ASPCommonsVariable*) solverCtx->getVariables().at(i).get();
    }
    std::vector<::reasoner::ASPCommonsTerm*> constraint;
    for (auto& c : calls) {
        constraint.push_back(toCommonsTerm(c->getConstraint()));
    }
    bool ret = this->solver->existsSolution(cVars, constraint);
    return ret;
}

bool ASPSolverWrapper::getSolutionImpl(
        SolverContext* ctx, const std::vector<std::shared_ptr<ProblemDescriptor>>& calls, std::vector<::reasoner::AnnotatedValVec*>& results)
{
    ASPSolverContext* solverCtx = static_cast<ASPSolverContext*>(ctx);

    if (!masterPlanLoaded) {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }

    auto cVars = std::vector<::reasoner::ASPCommonsVariable*>(solverCtx->getVariables().size());
    for (unsigned int i = 0; i < solverCtx->getVariables().size(); ++i) {
        cVars.at(i) = (::reasoner::ASPCommonsVariable*) solverCtx->getVariables().at(i).get();
    }

    std::vector<::reasoner::ASPCommonsTerm*> constraint;
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
::reasoner::ASPCommonsTerm* ASPSolverWrapper::toCommonsTerm(SolverTerm* term)
{
    auto tmp = (::reasoner::ASPCommonsTerm*) term;

    ::reasoner::ASPCommonsTerm* ret = new ::reasoner::ASPCommonsTerm();
    ret->setLifeTime(tmp->getLifeTime());
    ret->setQueryRule(tmp->getQueryRule());
    for (auto it : tmp->getRules()) {
        ret->addRule(it);
    }
    for (auto it : tmp->getFacts()) {
        ret->addFact(it);
    }
    ret->setId(tmp->getId());
    ret->setQueryId(tmp->getQueryId());
    ret->setExternals(tmp->getExternals());
    ret->setNumberOfModels(tmp->getNumberOfModels());
    ret->setProgramSection(tmp->getProgramSection());
    ret->setType(tmp->getType());
    return ret;
}

::reasoner::IASPSolver* ASPSolverWrapper::getSolver()
{
    return solver;
}

void ASPSolverWrapper::init(::reasoner::IASPSolver* solver)
{
    this->solver = solver;
    this->gen = new ASPGenerator(solver->WILDCARD_POINTER, solver->WILDCARD_STRING);
    this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(this->solver, this->gen);
}

void ASPSolverWrapper::integrateRules()
{
    for (auto query : this->getRegisteredQueries()) {
        for (auto rule : query->getRules()) {
            this->solver->add("planBase", {}, rule.c_str());
        }
    }
    this->solver->ground({{"planBase", {}}}, nullptr);
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

    this->integrateRules();
    for (auto query : this->getRegisteredQueries()) {
        query->reduceLifeTime();
    }
    auto result = this->solver->solve();
    this->removeDeadQueries();
    return result;
}

void alica::reasoner::ASPSolverWrapper::removeDeadQueries()
{
    this->solver->removeDeadQueries();
}

bool alica::reasoner::ASPSolverWrapper::registerQuery(shared_ptr<::reasoner::ASPQuery> query)
{
    return this->solver->registerQuery(query);
}

bool alica::reasoner::ASPSolverWrapper::unregisterQuery(shared_ptr<::reasoner::ASPQuery> query)
{
    return this->solver->unregisterQuery(query);
}

void alica::reasoner::ASPSolverWrapper::printStats()
{
    this->solver->printStats();
}

vector<shared_ptr<::reasoner::ASPQuery>> alica::reasoner::ASPSolverWrapper::getRegisteredQueries()
{
    return this->solver->getRegisteredQueries();
}
} // namespace reasoner

} /* namespace alica */

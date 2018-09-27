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

namespace alica
{
namespace reasoner
{
ASPSolverWrapper::ASPSolverWrapper(AlicaEngine *ae, std::vector<char const *> args)
    : alica::ISolver(ae)
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

bool ASPSolverWrapper::existsSolution(const VariableGrp &vars, std::vector<std::shared_ptr<ProblemDescriptor>> &calls)
{

    if (!masterPlanLoaded)
    {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }

    auto cVars = vector<shared_ptr<::reasoner::ASPCommonsVariable>>(vars.size());
    for (int i = 0; i < vars.size(); ++i)
    {
        cVars.at(i) = dynamic_pointer_cast<::reasoner::ASPCommonsVariable>(vars.at(i)->getSolverVar());
    }
    vector<shared_ptr<::reasoner::ASPCommonsTerm>> constraint;
    for (auto &c : calls)
    {
        if (!(dynamic_pointer_cast<alica::reasoner::ASPTerm>(c->getConstraint()) != 0))
        {
            cerr << "ASPSolverWrapper: Type of constraint not compatible with selected solver." << endl;
            continue;
        }
        constraint.push_back(toCommonsTerm(c->getConstraint()));
    }
    bool ret = this->solver->existsSolution(cVars, constraint);
    return ret;
}

bool ASPSolverWrapper::getSolution(const VariableGrp &vars, std::vector<std::shared_ptr<ProblemDescriptor>> &calls,
                 std::vector<void *> &results)
{
    if (!masterPlanLoaded)
    {
        this->planIntegrator->loadPlanTree(this->ae->getPlanBase()->getMasterPlan());
        masterPlanLoaded = true;
    }
    auto cVars = vector<shared_ptr<::reasoner::ASPCommonsVariable>>(vars.size());
    for (int i = 0; i < vars.size(); ++i)
    {
        cVars.at(i) = dynamic_pointer_cast<::reasoner::ASPCommonsVariable>(vars.at(i)->getSolverVar());
    }
    vector<shared_ptr<::reasoner::ASPCommonsTerm>> constraint;
    for (auto &c : calls)
    {
        if (!(dynamic_pointer_cast<alica::reasoner::ASPTerm>(c->getConstraint()) != 0))
        {
            cerr << "ASPSolverWrapper: Type of constraint not compatible with selected solver." << endl;
            continue;
        }
        constraint.push_back(toCommonsTerm(c->getConstraint()));
    }
    bool ret = this->solver->getSolution(cVars, constraint, results);
    return ret;
}

shared_ptr<SolverVariable> ASPSolverWrapper::createVariable(long id)
{
    return make_shared<alica::reasoner::ASPVariable>();
}

shared_ptr<::reasoner::ASPCommonsTerm> ASPSolverWrapper::toCommonsTerm(shared_ptr<SolverTerm> term)
{
    auto tmp = dynamic_pointer_cast<alica::reasoner::ASPTerm>(term);
    if (tmp == nullptr)
    {
        return nullptr;
    }
    shared_ptr<::reasoner::ASPCommonsTerm> ret = make_shared<::reasoner::ASPCommonsTerm>(tmp->getLifeTime());
    ret->setQueryRule(tmp->getQueryRule());
    for (auto it : tmp->getRules())
    {
        ret->addRule(it);
    }
    for (auto it : tmp->getFacts())
    {
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

shared_ptr<::reasoner::ASPCommonsVariable> ASPSolverWrapper::toCommonsVariable(shared_ptr<SolverVariable> var)
{
    auto tmp = dynamic_pointer_cast<alica::reasoner::ASPVariable>(var);
    if (tmp == nullptr)
    {
        return nullptr;
    }
    shared_ptr<::reasoner::ASPCommonsVariable> ret = make_shared<::reasoner::ASPCommonsVariable>();
    return ret;
}

::reasoner::IASPSolver *ASPSolverWrapper::getSolver()
{
    return solver;
}

void ASPSolverWrapper::init(::reasoner::IASPSolver *solver)
{
    this->solver = solver;
    this->gen = new ASPGenerator(solver->WILDCARD_POINTER, solver->WILDCARD_STRING);
    this->planIntegrator = make_shared<ASPAlicaPlanIntegrator>(this->solver, this->gen);
}

void ASPSolverWrapper::integrateRules()
{
    for (auto query : this->getRegisteredQueries())
    {
        for (auto rule : query->getRules())
        {
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
bool ASPSolverWrapper::validatePlan(Plan *plan)
{
    // adds all facts about the given plan tree in to clingo
    this->planIntegrator->loadPlanTree(plan);

    this->integrateRules();
    for (auto query : this->getRegisteredQueries())
    {
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
}

} /* namespace alica */

#include "asp_solver_wrapper/ASPAlicaPlanIntegrator.h"
#include "asp_solver_wrapper/ASPGenerator.h"

#include <engine/model/Behaviour.h>
#include <engine/model/Condition.h>
#include <engine/model/EntryPoint.h>
#include <engine/model/FailureState.h>
#include <engine/model/Plan.h>
#include <engine/model/PlanType.h>
#include <engine/model/PreCondition.h>
#include <engine/model/RuntimeCondition.h>
#include <engine/model/State.h>
#include <engine/model/SuccessState.h>
#include <engine/model/Synchronisation.h>
#include <engine/model/Task.h>

#include <CustomHashes.h>
#include <regex>

namespace alica
{
namespace reasoner
{

ASPAlicaPlanIntegrator::ASPAlicaPlanIntegrator(::reasoner::asp::Solver* solver, ASPGenerator* gen)
{
    this->solver = solver;
    this->gen = gen;
}

ASPAlicaPlanIntegrator::~ASPAlicaPlanIntegrator() {}

void ASPAlicaPlanIntegrator::loadPlanTree(const Plan* p)
{
    this->processedPlanIds.clear();

    /* Creates running plan instance of root plan and must
     * be called before processPlan(p), in order to have the
     * instanceElementHash correctly calculated for all child
     * plans.
     */
    uint64_t instanceElementHash = this->handleRunningPlan(p);

    // Start recursive integration of plan tree
    this->processPlan(p, instanceElementHash);

    // Ground the created plan base
    this->solver->ground({{"planBase", {}}}, nullptr);
    this->solver->ground({{"alicaBackground", {}}}, nullptr);
}

/**
 * Recursively integrates the given plan into the data-structures of the asp solver.
 * @param Plan p.
 * @return Returns true, if the processed plan is a tree.
 */
void ASPAlicaPlanIntegrator::processPlan(const Plan* p, uint64_t instanceElementHash)
{
    long currentPlanId = p->getId();
    if (find(processedPlanIds.begin(), processedPlanIds.end(), currentPlanId) != processedPlanIds.end()) { // already processed
        return;
    }

    processedPlanIds.push_back(currentPlanId);

#ifdef ASPAlicaPlanIntegratorDebug
    cout << "ASPAlicaPlanIntegrator: processing plan " << p->getName() << " (ID: " << p->getId() << ")" << endl;
#endif
    // add the plan
    this->solver->add("planBase", {}, gen->plan(p).c_str());

    // TODO: add pre- and run-time condition of plan
    this->processPreCondition(p->getPreCondition());
    this->processRuntimeCondition(p->getRuntimeCondition());

    // add entry points and their tasks
    for (const EntryPoint* entryPoint : p->getEntryPoints()) {
        const Task* task = entryPoint->getTask();

        // add task
        // TODO: what is, if we add the task two or more times to planBase?
        // TODO: should I also integrate the IDLE Task ?
        // - for runtime evaluation this should be part of the ALICA background knowledge
        this->solver->add("planBase", {}, gen->task(task).c_str());
        this->solver->add("planBase", {}, gen->hasTask(p, task).c_str());

        // add entry point
        this->solver->add("planBase", {}, gen->entryPoint(entryPoint).c_str());
        if (entryPoint->isSuccessRequired()) {
            this->solver->add("planBase", {}, gen->successRequired(entryPoint).c_str());
        }
        this->solver->add("planBase", {}, gen->hasInitialState(entryPoint, entryPoint->getState()).c_str());
        this->solver->add("planBase", {}, gen->hasMinCardinality(entryPoint, entryPoint->getMinCardinality()).c_str());
        this->solver->add("planBase", {}, gen->hasMaxCardinality(entryPoint, entryPoint->getMaxCardinality()).c_str());
        this->solver->add("planBase", {}, gen->hasEntryPoint(p, task, entryPoint).c_str());
        this->solver->add("planBase", {}, gen->hasEntryPoint(p, task, entryPoint).c_str());
    }

    // add state
    for (auto& state : p->getStates()) {

        this->solver->add("planBase", {}, gen->hasState(p, state).c_str());

        if (state->isFailureState()) {
            // add failure state
            this->solver->add("planBase", {}, gen->failureState(state).c_str());

            // TODO: handle post-condition of failure state
            // ((FailureState) state).getPostCondition();
        } else if (state->isSuccessState()) {
            // add success state
            this->solver->add("planBase", {}, gen->successState(state).c_str());

            // TODO: handle post-condition of success state
            // ((SuccessState) state).getPostCondition();
        } else // normal state
        {
            // add state
            this->solver->add("planBase", {}, gen->state(state).c_str());

            for (const AbstractPlan* abstractChildPlan : state->getPlans()) {
                if (const alica::Plan* childPlan = dynamic_cast<const alica::Plan*>(abstractChildPlan)) {
                    this->solver->add("planBase", {}, gen->hasPlan(state, childPlan).c_str());

                    instanceElementHash = handleRunningPlan(childPlan, state, instanceElementHash);

                    this->processPlan(childPlan, instanceElementHash);
                } else if (const alica::PlanType* childPlanType = dynamic_cast<const alica::PlanType*>(abstractChildPlan)) {
                    this->solver->add("planBase", {}, gen->planType(childPlanType).c_str());
                    this->solver->add("planBase", {}, gen->hasPlanType(state, childPlanType).c_str());

                    for (auto& childPlan : childPlanType->getPlans()) {
                        this->solver->add("planBase", {}, gen->hasRealisation(childPlanType, childPlan).c_str());

                        instanceElementHash = handleRunningPlan(childPlan, state, childPlanType, instanceElementHash);

                        this->processPlan(childPlan, instanceElementHash);
                    }
                } else if (const alica::Behaviour* childBehaviour = dynamic_cast<const alica::Behaviour*>(abstractChildPlan)) {
                    // TODO: Handle Behaviour
                    this->solver->add("planBase", {}, gen->behaviour(childBehaviour).c_str());
                    this->solver->add("planBase", {}, gen->hasBehaviour(state, childBehaviour).c_str());
                    // TODO handle pre- and runtime condition of beh conf
                }
            }
        }

        // add state transition relations
        for (auto& inTransition : state->getInTransitions()) {
            this->solver->add("planBase", {}, gen->hasInTransition(state, inTransition).c_str());
        }
        for (auto& outTransition : state->getOutTransitions()) {
            this->solver->add("planBase", {}, gen->hasOutTransition(state, outTransition).c_str());
        }
    }

    // add transitions
    for (auto& transition : p->getTransitions()) {
        this->solver->add("planBase", {}, gen->transition(transition).c_str());

        // TODO: handle pre-conditions of transitions
    }

    // add synchronisations
    for (auto& syncTransition : p->getSynchronisations()) {
        this->solver->add("planBase", {}, gen->synchronisation(syncTransition).c_str());
        for (auto& transition : syncTransition->getInSync()) {
            this->solver->add("planBase", {}, gen->hasSynchedTransition(syncTransition, transition).c_str());
        }
        // TODO: maybe it is nice to have the timeouts of a sync transition
    }
}

void ASPAlicaPlanIntegrator::processPreCondition(const PreCondition* cond)
{
    if (!cond || !cond->isEnabled()) {
        return;
    }

    // alica program facts
    this->solver->add("planBase", {}, gen->preCondition(cond).c_str());
    if (const Plan* plan = dynamic_cast<const Plan*>(cond->getAbstractPlan())) {
        this->solver->add("planBase", {}, gen->hasPreCondition(plan, cond).c_str());
    }

    // asp encoded precondition
    if (cond->getConditionString() != "") {
        const std::string& condString = cond->getConditionString();

        std::cout << "ASP-Integrator: " << gen->preConditionHolds(cond) << std::endl;
        this->solver->add("planBase", {}, gen->preConditionHolds(cond).c_str());

        // analysis of asp encoded precondition, because of non-local in relations
        handleCondString(condString, "preCond", cond);
    }
}

void ASPAlicaPlanIntegrator::processRuntimeCondition(const RuntimeCondition* cond)
{
    // TODO finish RuntimeCondition
    if (!cond) {
        return;
    }

    // alica program facts
    this->solver->add("planBase", {}, gen->runtimeCondition(cond).c_str());
    if (const Plan* plan = dynamic_cast<const Plan*>(cond->getAbstractPlan())) {
        this->solver->add("planBase", {}, gen->hasRuntimeCondition(plan, cond).c_str());
    }

    if (cond->getConditionString() != "") {
        const std::string& condString = cond->getConditionString();

        std::cout << "ASP-Integrator: " << gen->runtimeConditionHolds(cond) << std::endl;
        this->solver->add("planBase", {}, gen->runtimeConditionHolds(cond).c_str());

        // analysis of asp encoded precondition, because of non-local in relations
        handleCondString(condString, "runtimeCond", cond);
    }
}

uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(const Plan* rootPlan)
{
    uint64_t instanceElementHash = essentials::CustomHashes::FNV_OFFSET ^ rootPlan->getId() * essentials::CustomHashes::FNV_MAGIC_PRIME;
    this->solver->add("planBase", {}, gen->hasPlanInstance(rootPlan, instanceElementHash).c_str());
    this->solver->add("planBase", {}, gen->runningPlan(instanceElementHash).c_str());
    return instanceElementHash;
}

uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(const Plan* plan, const State* state, uint64_t instanceElementHash)
{
    instanceElementHash = instanceElementHash ^ state->getId() * essentials::CustomHashes::FNV_MAGIC_PRIME;
    this->solver->add("planBase", {}, gen->hasRunningPlan(state, instanceElementHash).c_str());
    this->solver->add("planBase", {}, gen->hasPlanInstance(plan, instanceElementHash).c_str());
    this->solver->add("planBase", {}, gen->runningPlan(instanceElementHash).c_str());
    return instanceElementHash;
}

uint64_t ASPAlicaPlanIntegrator::handleRunningPlan(const Plan* plan, const State* state, const PlanType* planType, uint64_t instanceElementHash)
{
    instanceElementHash = instanceElementHash ^ planType->getId() * essentials::CustomHashes::FNV_MAGIC_PRIME;
    instanceElementHash = handleRunningPlan(plan, state, instanceElementHash);
    this->solver->add("planBase", {}, gen->hasRunningRealisation(planType, instanceElementHash).c_str());
    return instanceElementHash;
}

void ASPAlicaPlanIntegrator::handleCondString(const std::string& condString, std::string prefix, const Condition* cond)
{
    // analysis of asp encoded precondition, because of non-local in relations
    std::regex words_regex("((\\s|,){1}|^)in\\((\\s*)([A-Z]+(\\w*))(\\s*),(\\s*)([a-z]+(\\w*))(\\s*)(,(\\s*)([a-zA-Z]+("
                           "\\w*))(\\s*)){2}\\)");
    auto words_begin = std::sregex_iterator(condString.begin(), condString.end(), words_regex);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string inPredicateString = match.str();
        //				std::cout << "ASPAlicaPlanInegrator: ALL MATCH>>>>>>" << inPredicateString <<
        //"<<<<<<"
        //<< std::endl;
        ;
        size_t start = inPredicateString.find(',');
        size_t end = std::string::npos;
        std::string plan = "";
        std::string task = "";
        std::string state = "";
        // plan
        if (start != std::string::npos) {
            end = inPredicateString.find(',', start + 1);
            if (end != std::string::npos) {
                plan = inPredicateString.substr(start + 1, end - start - 1);
                this->solver->add("planBase", {}, gen->inRefPlan(prefix, cond, plan).c_str());
                //						std::cout << "ASPAlicaPlanInegrator: PLAN MATCH>>>>>>" << plan
                //<<
                //"<<<<<<" << std::endl;
            }
        }
        // task
        start = end + 1;
        if (start != std::string::npos) {
            end = inPredicateString.find(',', start);
            if (end != std::string::npos) {
                task = inPredicateString.substr(start, end - start);
                task = essentials::Configuration::trim(task);
                //						std::cout << "ASPAlicaPlanInegrator: TASK MATCH>>>>>>" << task
                //<<
                //"<<<<<<" << std::endl;
                if (islower(task.at(0))) {
                    this->solver->add("planBase", {}, gen->inRefPlanTask(prefix, cond, plan, task).c_str());
                } else {
                    task = "";
                }
            }
        }
        // state
        start = end + 1;
        if (start != std::string::npos) {
            end = inPredicateString.find(')', start);
            if (end != std::string::npos) {
                state = inPredicateString.substr(start, end - start);
                state = essentials::Configuration::trim(state);
                //						std::cout << "ASPAlicaPlanInegrator: STATE MATCH>>>>>>" <<
                // state
                //<< "<<<<<<" << std::endl;
                if (islower(state.at(0))) {
                    this->solver->add("planBase", {}, gen->inRefPlanState(prefix, cond, plan, state).c_str());
                } else {
                    state = "";
                }
            }
        }
        // plan, task, state
        if (task != "" && state != "") {
            this->solver->add("planBase", {}, gen->inRefPlanTaskState(prefix, cond, plan, task, state).c_str());
        }
    }
}

} /* namespace reasoner */
} /* namespace alica */

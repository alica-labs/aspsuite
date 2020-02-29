#include "knowledge_manager/ASPKnowledgeManager.h"
#include <reasoner/asp/AnnotatedValVec.h>
#include <reasoner/asp/ExtensionQuery.h>
#include <reasoner/asp/FilterQuery.h>
#include <reasoner/asp/Solver.h>
#include <reasoner/asp/Term.h>
#include <reasoner/asp/Variable.h>

#include <algorithm>

#define ASPKM_DEBUG

namespace knowledge_manager
{
ASPKnowledgeManager::ASPKnowledgeManager()
        : solver(nullptr)
        , isDirty(false)
{
}

ASPKnowledgeManager::~ASPKnowledgeManager() {}

std::vector<std::string> ASPKnowledgeManager::filterModel(const std::string& queryValue)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto term = new ::reasoner::asp::Term();
    term->setLifeTime(1);
    term->addQueryValue(queryValue);
    term->setType(reasoner::asp::QueryType::Filter);

    std::shared_ptr<reasoner::asp::FilterQuery> fq = std::make_shared<reasoner::asp::FilterQuery>(this->solver->generateQueryID(), this->solver, term);
    this->solver->registerQuery(fq);
    if (this->solver->solve()) {
        std::cout << "[ASPKnowledgeManager] Solving success!" << std::endl;
    } else {
        std::cout << "[ASPKnowledgeManager] Solving unsuccessful!" << std::endl;
    }
    this->solver->unregisterQuery(fq->getQueryID());

    // extract results from query
    std::vector<std::string> results;
    for (auto& mapping : fq->getQueryResultMappings()) {
        for (auto& entry : mapping) {
            for (auto& resultValue : entry.second) {
                results.push_back(resultValue.to_string());
            }
        }
    }

    delete term;
    return results;
}

/**
 * Combines information into a term, wraps it in a query and registers it in the solver
 * @return id of query
 */
int ASPKnowledgeManager::addInformation(std::vector<std::string>& information, int lifetime)
{
    std::lock_guard<std::mutex> lock(mtx);
    ::reasoner::asp::Term* term = new ::reasoner::asp::Term(lifetime);
    for (auto inf : information) {
        term->addFact(inf);
    }
    int queryId = this->solver->generateQueryID();
    std::shared_ptr<::reasoner::asp::Query> query = std::make_shared<::reasoner::asp::ExtensionQuery>(queryId, this->solver, term);
    this->solver->registerQuery(query);
    return queryId;
}

void ASPKnowledgeManager::updateExternals(std::shared_ptr<std::map<std::string, bool> > externals) {
    std::lock_guard<std::mutex> lock(mtx);
    this->solver->handleExternals(externals);
}

/**
 * Adds the given rules permanently to the base program section and grounds it.
 * @param rules
 */
void ASPKnowledgeManager::addRulesPermanent(const std::string& programSection, std::vector<std::string>& rules)
{
    // Future Work: change to background knowledge query
    std::stringstream rulesStream;
    rulesStream << "#program " << programSection << ".\n";
    for (const auto& info : rules) {
        rulesStream << info << "\n";
    }
//    std::cout << "[ASPKnowledgeManager] " << rulesStream.str() << std::endl;
    std::lock_guard<std::mutex> lock(mtx);
    this->solver->add(programSection.c_str(), {}, rulesStream.str().c_str());
    this->solver->ground({{programSection.c_str(), {}}}, nullptr);
}

/**
 * Removes a specified query from solver
 */
void ASPKnowledgeManager::revoke(int queryID)
{
    std::lock_guard<std::mutex> lock(mtx);
    solver->unregisterQuery(queryID);
}

/**
 * Sets the solver because it cannot be instantiated at construction time.
 */
void ASPKnowledgeManager::setSolver(reasoner::asp::Solver* solver)
{
    this->solver = solver;
}

void ASPKnowledgeManager::add(char const* name, Clingo::StringSpan const& params, char const* part)
{
    this->solver->add(name, params, part);
}

void ASPKnowledgeManager::ground(Clingo::PartSpan vec, Clingo::GroundCallback callBack)
{
    this->solver->ground(vec, callBack);
}

bool ASPKnowledgeManager::solve()
{
    return this->solver->solve();
}

Clingo::Symbol ASPKnowledgeManager::parseValue(std::string const& str)
{
    return this->solver->parseValue(str);
}

//
// reasoner::asp::Solver* ASPKnowledgeManager::getSolver()
//{
//    return solver;
//}

} /* namespace knowledge_manager */

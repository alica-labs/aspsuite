#include "knowledge_manager/ASPKnowledgeManager.h"
#include <reasoner/asp/AnnotatedValVec.h>
#include <reasoner/asp/ExtensionQuery.h>
#include <reasoner/asp/Solver.h>
#include <reasoner/asp/Term.h>
#include <reasoner/asp/Variable.h>

#include <algorithm>

#define ASPKM_DEBUG

namespace knowledge_manager
{
ASPKnowledgeManager::ASPKnowledgeManager()
        : solver(nullptr)
{
}

ASPKnowledgeManager::~ASPKnowledgeManager() {}

/**
 * Solves the current ASP Program of the Knowledge Base according to specified queryRule
 */
std::vector<std::string> ASPKnowledgeManager::solve(std::string queryRule, std::string programSection)
{
    auto vars = std::vector<reasoner::asp::Variable*>();
    vars.push_back(new reasoner::asp::Variable());
    auto terms = std::vector<reasoner::asp::Term*>();
    std::vector<reasoner::asp::AnnotatedValVec*> results;

    auto ret = std::vector<std::string>();
    auto term = new ::reasoner::asp::Term();
    term->setQueryRule(queryRule);
    term->setType(::reasoner::asp::QueryType::Filter);
    term->setProgramSection(programSection);

    std::lock_guard<std::mutex> lock(mtx);
    term->setId(this->solver->generateQueryID());
    terms.push_back(term);
    this->solver->getSolution(vars, terms, results);

    if (results.size() > 0) {
#ifdef ASPKM_DEBUG
        std::cout << "[ASPKnowledgeManager] Found Result!" << std::endl;
#endif
        for (auto res : results) {
            for (size_t i = 0; i < res->factQueryValues.size(); ++i) {
                for (size_t j = 0; j < res->factQueryValues.at(i).size(); ++j) {
                    auto elem = res->factQueryValues.at(i).at(j);
                    std::cout << "[ASPKnowledgeManager] " << i << "," << j << ", " << elem << std::endl;
                    this->currentSolution.push_back(elem);
                }
            }
        }
    }

    for (auto var : vars) {
        delete var;
    }
    for (auto t : terms) {
        delete t;
    }

    return this->currentSolution;
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

reasoner::asp::Solver* ASPKnowledgeManager::getSolver()
{
    return solver;
}

} /* namespace knowledge_manager */

#include "reasoner/asp/IncrementalQueryWrapper.h"
#include "reasoner/asp/Solver.h"
namespace reasoner
{
namespace asp
{
IncrementalQueryWrapper::IncrementalQueryWrapper(::reasoner::asp::Solver* solver, std::string queryExternalPrefix)
        : solver(solver)
        , queryExternalPrefix(std::move(queryExternalPrefix))
        , baseQuery(nullptr)
{
    // this->queries.push_back(this);
}

void IncrementalQueryWrapper::addQueryForHorizon(int horizon, ::reasoner::asp::Term* term)
{
    if (!this->baseQuery && horizon == 0) {
        this->baseQuery = std::make_shared<::reasoner::asp::IncrementalExtensionQuery>(solver, term, this->queryExternalPrefix, horizon);
    } else if (this->queries.find(horizon) == this->queries.end()) {
        this->queries.emplace(horizon, std::make_shared<::reasoner::asp::IncrementalExtensionQuery>(solver, term, this->queryExternalPrefix, horizon));
    }
}

bool IncrementalQueryWrapper::isPresent(int horizon)
{
    return this->queries.size() >= horizon;
}

void IncrementalQueryWrapper::activate(int horizon)
{
    std::shared_ptr<IncrementalExtensionQuery> query;
    if (horizon == 0) {
        query = this->baseQuery;
        std::cout << "incwrapper: activate query with id " << this->baseQuery->getTerm()->getQueryId() << std::endl;
    } else {
        query = this->queries.at(horizon);
    }
    std::cout << "Activating query with id " << query->getTerm()->getQueryId() << std::endl;
//    auto queries = this->solver->getRegisteredQueries2();
//    for(auto q : queries) {
//        std::cout << "Solver queries: id " << q->getTerm()->getQueryId() << std::endl;
//    }
    this->solver->assignExternal(*(query->external), Clingo::TruthValue::True);
//    query->getSolver()->assignExternal();
}

void IncrementalQueryWrapper::deactivate(int horizon)
{
    std::shared_ptr<IncrementalExtensionQuery> query;
    if (horizon == 0) {
        query = this->baseQuery;
    } else {
        query = this->queries.at(horizon);
    }
    query->getSolver()->assignExternal(*(query->external), Clingo::TruthValue::False);
}

void IncrementalQueryWrapper::cleanUp()
{
    // only contains step queries, base is currently handled in inc-problem - refactor!
    for (const auto& query : this->queries) {
        query.second->getSolver()->assignExternal(*(query.second->external), Clingo::TruthValue::False);
    }
}

void IncrementalQueryWrapper::clear()
{
    this->queries.clear();
}

/**
 * FIXME remove as soon as check terms can be created generically
 */
std::string IncrementalQueryWrapper::getQueryExternalPrefix() const
{
    return this->queryExternalPrefix;
}
}
}
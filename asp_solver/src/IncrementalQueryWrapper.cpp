#include "reasoner/asp/IncrementalQueryWrapper.h"
#include "reasoner/asp/Solver.h"
namespace reasoner
{
namespace asp
{
IncrementalQueryWrapper::IncrementalQueryWrapper(asp::Solver* solver, std::string queryExternalPrefix)
        : solver(solver)
        , queryExternalPrefix(std::move(queryExternalPrefix))
{
    // this->queries.push_back(this);
}

void IncrementalQueryWrapper::addQueryForHorizon(int horizon, reasoner::asp::Term* term)
{
    if (this->queries.find(horizon) == this->queries.end()) {
        this->queries.emplace(horizon, std::make_shared<reasoner::asp::IncrementalExtensionQuery>(solver, term, this->queryExternalPrefix, horizon));
    }
}

bool IncrementalQueryWrapper::isPresent(int horizon)
{
    return this->queries.size() >= horizon;
}

void IncrementalQueryWrapper::activate(int horizon)
{
    auto query = this->queries.at(horizon);
    query->getSolver()->assignExternal(*(query->external), Clingo::TruthValue::True);
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
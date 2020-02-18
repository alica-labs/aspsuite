#include "reasoner/asp/FilterQuery.h"

#include "reasoner/asp/Solver.h"
#include "reasoner/asp/TruthValue.h"

namespace reasoner
{
namespace asp
{

FilterQuery::FilterQuery(int queryID, Solver* solver, Term* term)
        : Query(queryID, solver, term, QueryType::Filter)
{
    this->addQueryValues(term->getFacts());
}

bool FilterQuery::queryValuesExistForAtLeastOneModel()
{
    for (const auto& mapping : this->queryResultMappings) {
        for (const auto& queryValue : mapping) {
            if (!queryValue.second.empty()) {
                return true;
            }
        }
    }
    return false;
}

bool FilterQuery::queryValuesExistForAllModels()
{
    for (auto& mapping : this->queryResultMappings) {
        for (auto& entry : mapping) {
            if (entry.second.empty()) {
                return false;
            }
        }
    }
    return true;
}

void FilterQuery::removeExternal()
{ // NOOP in case of FilterQuery
}

std::vector<std::pair<Clingo::Symbol, TruthValue>> FilterQuery::getTruthValues(int modelIdx)
{
    std::vector<std::pair<Clingo::Symbol, TruthValue>> ret;
    auto& mapping = this->queryResultMappings[modelIdx];
    for (auto iter : mapping) {
        if (iter.second.empty()) {
            ret.emplace_back(iter.first, TruthValue::Unknown);
        } else {
            if (iter.second.at(0).is_positive()) {
                ret.emplace_back(iter.first, TruthValue::True);
            } else {
                ret.emplace_back(iter.first, TruthValue::False);
            }
        }
    }
    return ret;
}

} /* namespace asp */
} /* namespace reasoner */

#pragma once

#include "IncrementalExtensionQuery.h"
#include <set>

namespace reasoner
{
namespace asp
{
/**
 * A container for Incremental queries.
 * Handles assigning of IDs and externals
 */
class IncrementalQueryWrapper
{
public:
    IncrementalQueryWrapper(asp::Solver* solver, std::string queryExternalPrefix);

    void addQueryForHorizon(int horizon, reasoner::asp::Term* term);

    std::string getQueryExternalPrefix() const;

    bool isPresent(int horizon);
    void activate(int horizon);
    void cleanUp();
    void clear();

private:
    std::map<int, std::shared_ptr<reasoner::asp::IncrementalExtensionQuery>> queries;

    asp::Solver* solver;

    const std::string queryExternalPrefix;

};
}
}
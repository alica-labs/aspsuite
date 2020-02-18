#pragma once
#include "reasoner/asp/Query.h"

#include "reasoner/asp/Term.h"
#include "reasoner/asp/TruthValue.h"

#include <clingo.hh>

namespace reasoner
{
namespace asp
{

class Solver;
class FilterQuery : public Query
{
public:
    FilterQuery(int queryID, Solver* solver, Term* term);
    ~FilterQuery() override = default;

    bool queryValuesExistForAllModels();
    bool queryValuesExistForAtLeastOneModel();
    std::vector<std::pair<Clingo::Symbol, TruthValue>> getTruthValues(int modelIdx = 0);

    void removeExternal() override; /**< NOOP in case of ASPFilterQuery */
private:

};
} /* namespace asp */
} /* namespace reasoner */

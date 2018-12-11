#pragma once

#include "reasoner/asp/Enums.h"
#include "reasoner/asp/Query.h"
#include "reasoner/asp/Term.h"

#include <clingo.hh>

namespace reasoner
{
namespace asp
{

class Solver;
class FilterQuery : public Query
{
public:
    FilterQuery(Solver* solver, Term* term);
    virtual ~FilterQuery();

    bool factsExistForAllModels();
    bool factsExistForAtLeastOneModel();

    // NOOP in case of ASPFilterQuery
    void removeExternal();

    std::vector<std::pair<Clingo::Symbol, TruthValue>> getTruthValues();

    void onModel(Clingo::Model& clingoModel);

private:
    void addQueryValues(std::vector<std::string> queryString);
};
} /* namespace asp */
} /* namespace reasoner */

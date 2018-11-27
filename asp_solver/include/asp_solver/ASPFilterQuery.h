#pragma once

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>
#include <asp_commons/ASPTruthValue.h>

#include <clingo.hh>

namespace reasoner
{

class ASPSolver;
class ASPFilterQuery : public ASPQuery
{
public:
    ASPFilterQuery(ASPSolver* solver, ASPCommonsTerm* term);
    virtual ~ASPFilterQuery();

    bool factsExistForAllModels();
    bool factsExistForAtLeastOneModel();

    // NOOP in case of ASPFilterQuery
    void removeExternal();

    std::vector<std::pair<Clingo::Symbol, ASPTruthValue>> getASPTruthValues();

    void onModel(Clingo::Model& clingoModel);
private:
    void addQueryValues(std::vector<std::string> queryString);
};

} // namespace reasoner
/* namespace reasoner */

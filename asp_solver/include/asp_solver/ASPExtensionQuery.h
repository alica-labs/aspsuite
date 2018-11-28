#pragma once

#include <asp_commons/ASPCommonsTerm.h>
#include <asp_commons/ASPQuery.h>
#include <asp_commons/ASPQueryType.h>

#include <clingo.hh>

#include <memory>

//#define ASPVARIABLEQUERY_DEBUG

namespace reasoner
{
class ASPSolver;
class ASPExtensionQuery : public ASPQuery
{
public:
    ASPExtensionQuery(ASPSolver* solver, reasoner::ASPCommonsTerm* term);
    virtual ~ASPExtensionQuery();
    ASPQueryType getType();
    void removeExternal();
    void onModel(Clingo::Model& clingoModel);

private:
    void createHeadQueryValues(std::vector<std::string> queryVec);
    std::vector<std::string> createHeadPredicates(std::string head);
    std::string expandQueryRuleModuleProperty(std::string rule);
    std::string expandRuleModuleProperty(std::string rule);
    std::string expandFactModuleProperty(std::string fact);
    void replaceFittingPredicate(std::string& ruleBody, std::string predicate);
    std::string replaceHeadPredicates(std::string head);
    std::shared_ptr<Clingo::Symbol> external;
    std::string queryProgramSection;
    std::string externalName;

    /**
     * Encapsulates the query in an extra program section including an external (the query external, e.g., query1).
     * The query external is added to every rule in the query, in order to guard it: bla bla :- blub blub, query1.
     * Furthermore, it encapsulates queried rules/ ruleheads with a query1(...) in order to guarantee the module property.
     *
     * Therefore, it utilises the methods above... expandBlaBla()
     */
    void createProgramSection();
};

} /* namespace reasoner */

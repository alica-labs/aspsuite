#pragma once

#include "reasoner/asp/Enums.h"
#include "reasoner/asp/Query.h"
#include "reasoner/asp/Term.h"

#include <clingo.hh>

#include <memory>

//#define ASPVARIABLEQUERY_DEBUG

namespace reasoner
{
namespace asp
{
class Solver;
class ExtensionQuery : public Query
{
public:
    ExtensionQuery(Solver* solver, Term* term);

    virtual ~ExtensionQuery();
    virtual void removeExternal();
    virtual void onModel(Clingo::Model& clingoModel);
    std::shared_ptr<Clingo::Symbol> external;

protected:


    std::string queryProgramSection;
    std::string externalName;

    std::string createKBCapturingRule(const std::string& headPredicateName, int arity, const std::string& querySection);

    std::string expandFactModuleProperty(std::string fact);
    std::string expandRuleModuleProperty(const std::string& rule, const std::string& querySection);


    /**
     * Encapsulates the query in an extra program section including an external (the query external, e.g., query1).
     * The query external is added to every rule in the query, in order to guard it: bla bla :- blub blub, query1.
     * Furthermore, it encapsulates queried rules/ ruleheads with a query1(...) in order to guarantee the module property.
     *
     * Therefore, it utilises the methods above... expandBlaBla()
     */
    virtual void createProgramSection();
};

} /* namespace asp */
} /* namespace reasoner */

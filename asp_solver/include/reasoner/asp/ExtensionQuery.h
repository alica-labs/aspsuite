#pragma once

#include "reasoner/asp/Enums.h"
#include "reasoner/asp/Query.h"
#include "reasoner/asp/Term.h"

#include <clingo.hh>

#include <memory>
#include <unordered_set>

#define EXTENSIONQUERY_DEBUG

namespace reasoner
{
namespace asp
{
class Solver;
class ExtensionQuery : public Query
{
public:
    ExtensionQuery(int queryID, Solver* solver, Term* term);
    virtual ~ExtensionQuery() = default;

    virtual void removeExternal();
    virtual void onModel(Clingo::Model& clingoModel);


protected:
    struct Predicate
    {
        std::string name;
        std::string parameters;
        int arity;
        size_t parameterStartIdx;
        size_t parameterEndIdx;

        Predicate()
                : name("")
                , parameters("")
                , arity(0)
                , parameterStartIdx(0)
                , parameterEndIdx(0)
        {
        }
    };

    enum Separator
    {
        Comma,
        Colon,
        Semicolon,
        None
    };

    std::string trim(const std::string& str);
    bool isMinOrMax(std::string rule, size_t openingCurlyBracesIdx);
    size_t findNextChar(const std::string& predicate, const std::string& chars, size_t end, size_t start = 0);
    size_t findNextCharNotOf(const std::string& predicate, const std::string& chars, size_t end, size_t start = 0);
    size_t findImplication(const std::string& rule);
    size_t findMatchingClosingBrace(const std::string& predicate, size_t openingBraceIdx);
    std::pair<Separator, size_t> determineFirstSeparator(std::pair<Separator, size_t> a, std::pair<Separator, size_t> b);

    void rememberPredicate(std::string predicateName, int arity);
    Predicate extractConstant(std::string rule, size_t constantEndIdxPlusOne);
    Predicate extractPredicate(std::string rule, size_t parameterStartIdx);
    void extractHeadPredicates(const std::string& rule);
    bool lookUpPredicate(const std::string& predicateName, int arity);

    std::string createVariableParameters(int arity);
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
    virtual void generateQueryProgram();

    std::map<std::string, std::unordered_set<int>> predicatesToAritiesMap;

    std::string queryProgramSection;
    std::string externalName;
    std::string queryProgram;
    std::shared_ptr<Clingo::Symbol> external;
};

} /* namespace asp */
} /* namespace reasoner */

#pragma once

#include "PredicateContainer.h"
#include "QueryType.h"

#include <clingo.hh>

#include <map>
#include <memory>
#include <string>

//#define ASPQUERY_DEBUG

namespace reasoner
{

namespace asp
{
class Term;
class AnnotatedValVec;
class Solver;
class Query : public PredicateContainer
{
public:
    Query(int queryID, Solver* solver, Term* term, QueryType type);
    virtual ~Query() = default;

    // CONFIGURATION and STATE STUFF
    Term* getTerm();
    Solver* getSolver();
    int getLifeTime();
    void setLifeTime(int newLifeTime);
    void reduceLifeTime();
    QueryType getType();
    int getQueryID() const;

    // RESULT STUFF
    void addQueryValues(const std::vector<std::string>& queryVec);
    void addQueryResultMapping(int modelIdx, Clingo::Symbol key, Clingo::Symbol value);
    const std::vector<std::map<Clingo::Symbol, Clingo::SymbolVector>>& getQueryResultMappings();
    const std::vector<Clingo::SymbolVector>& getCurrentModels();

    std::string toString(bool verbose = false);

    /**
     * Called if the lifetime is over, or if the
     * Query is unregistered. Must be overwritten by derived query
     * classes.
     */
    virtual void removeExternal() = 0;

    /**
     * Callback for Clingo to return a model.
     * @param clingoModel
     */
    virtual void onModel(Clingo::Model& clingoModel);
protected:
    /**
     * Determines whether two symbols mean the same, taking wildcard
     * placeholders into account.
     * @param value1
     * @param value2
     * @return True, if they match. False, otherwise.
     */
    static bool match(Clingo::Symbol value1, Clingo::Symbol value2);

    int queryID;
    Solver* solver;
    Term* term;
    QueryType type;
    int lifeTime; /**< -1 means forever until unregistered, X > 0 means it is used X times */

    Clingo::SymbolVector queriedValues;

    /**
     * Vector of  models, received from the last solve call.
     * The order matches the order in the queryResultMappings vector.
     */
    std::vector<Clingo::SymbolVector> currentModels;

    /**
     * The vector matches the order of models in "currentModels".
     * The map is organised as follows:
     * key := queriedValue , vector<value> := result values which satisfies the key
     */
    std::vector<std::map<Clingo::Symbol, Clingo::SymbolVector>> queryResultMappings;

};
} /* namespace asp */
} /* namespace reasoner */

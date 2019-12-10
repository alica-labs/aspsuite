#pragma once

#include "Enums.h"

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
class Query
{
public:
    Query(int queryID, Solver* solver, Term* term, QueryType type);
    virtual ~Query();

    std::shared_ptr<std::vector<Clingo::SymbolVector>> getCurrentModels();

    int getLifeTime();
    void setLifeTime(int lifeTime);
    void reduceLifeTime();

    virtual void onModel(Clingo::Model& clingoModel) = 0;

    std::map<Clingo::Symbol, Clingo::SymbolVector>& getHeadValues();

    Solver* getSolver();
    Term* getTerm();

    std::string getProgramSection();
    void setProgramSection(std::string programSection);
    virtual void removeExternal() = 0;

    std::string toString();
    QueryType getType();
    int getQueryID() const;
    void saveHeadValuePair(Clingo::Symbol key, Clingo::Symbol value);
    bool checkMatchValues(Clingo::Symbol value1, Clingo::Symbol value2);

protected:
    /**
     * queryString is used to ask the solver if specific predicates are true.
     * predicates are separated by "," meaning all of them will be in the same rule and ";"
     * meaning that there is a rule for every predicate
     */
    Solver* solver;
    Term* term;
    QueryType type;
    int queryID;
    std::shared_ptr<std::vector<Clingo::SymbolVector>> currentModels;
    // key := headValue , value := values which satisfies it
    std::map<Clingo::Symbol, Clingo::SymbolVector> headValues;
    // lifeTime == 1 => query is used once
    // lifeTime == x => query is used x times
    // LifeTime == -1 => query is used until unregistered
    int lifeTime;
    std::string programSection;
    std::string backgroundKnowledgeFilename;
};
} /* namespace asp */
} /* namespace reasoner */

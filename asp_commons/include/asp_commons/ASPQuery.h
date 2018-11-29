#pragma once

#include "asp_commons/ASPQueryType.h"
#include <clingo.hh>

#include <map>
#include <memory>
#include <string>

//#define ASPQUERY_DEBUG

namespace reasoner
{

class ASPCommonsTerm;
class AnnotatedValVec;
class IASPSolver;
class ASPQuery
{
public:
    ASPQuery(IASPSolver* solver, ASPCommonsTerm* term);
    virtual ~ASPQuery();

    std::shared_ptr<std::vector<Clingo::SymbolVector>> getCurrentModels();

    int getLifeTime();
    void setLifeTime(int lifeTime);
    void reduceLifeTime();

    virtual void onModel(Clingo::Model& clingoModel) = 0;

    std::map<Clingo::Symbol, Clingo::SymbolVector>& getHeadValues();

    IASPSolver* getSolver();
    ASPCommonsTerm* getTerm();

    std::string getProgramSection();
    void setProgramSection(std::string programSection);
    virtual void removeExternal() = 0;

    std::string toString();
    ASPQueryType getType();
    void saveHeadValuePair(Clingo::Symbol key, Clingo::Symbol value);
    bool checkMatchValues(Clingo::Symbol value1, Clingo::Symbol value2);

protected:
    /**
     * queryString is used to ask the solver if specific predicates are true.
     * predicates are separated by "," meaning all of them will be in the same rule and ";"
     * meaning that there is a rule for every predicate
     */
    IASPSolver* solver;
    std::shared_ptr<std::vector<Clingo::SymbolVector>> currentModels;
    // key := headValue , value := values which satisfies it
    std::map<Clingo::Symbol, Clingo::SymbolVector> headValues;
    // lifeTime == 1 => query is used once
    // lifeTime == x => query is used x times
    // LifeTime == -1 => query is used until unregistered
    int lifeTime;
    std::string programSection;
    ASPCommonsTerm* term;
    ASPQueryType type;
    std::string backgroundKnowledgeFilename;
};

} /* namespace reasoner */

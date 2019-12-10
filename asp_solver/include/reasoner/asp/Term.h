#pragma once

#include "reasoner/asp/Enums.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace reasoner
{
namespace asp
{

class Term : public std::enable_shared_from_this<Term>
{
public:
    Term(int lifeTime = 1);
    virtual ~Term();
    void addRule(std::string rule);
    std::vector<std::string> getRuleHeads();
    std::vector<std::string> getRuleBodies();
    std::string getProgramSection();
    void setProgramSection(std::string programSection);
    std::vector<std::string> getProgramSectionParameters();
    void addProgramSectionParameter(std::string param);
    int getLifeTime();
    void setLifeTime(int lifeTime);
    std::vector<std::string> getRules();
    void addFact(std::string fact); /**< Fact needs to end with a '.'! */
    std::vector<std::string> getFacts();
    void setExternals(std::shared_ptr<std::map<std::string, bool>> externals);
    std::shared_ptr<std::map<std::string, bool>> getExternals();
    std::string getNumberOfModels();
    void setNumberOfModels(std::string numberOfModels);
    QueryType getType();
    void setType(QueryType type);
    long getId();
    void setId(long id);
//    int getQueryId();
//    void setQueryId(int queryId);
    std::string getQueryRule();
    void setQueryRule(std::string queryRule);

private:
    std::string numberOfModels;
    std::string queryRule;
    std::vector<std::string> rules;
    std::vector<std::string> heads;
    std::vector<std::string> bodies;
    std::string programSection;
    long id;
    /**
     * The query id has to be added to any predicate which is added to the program, naming rule
     * heads and facts!
     * An unique id is given by the ASPSolver!
     */
//    int queryId;
    int lifeTime;
    std::vector<std::string> facts;
    std::vector<std::string> programSectionParameters;
    std::shared_ptr<std::map<std::string, bool>> externals;
    QueryType type;
};

} /* namespace asp */
} /* namespace reasoner */

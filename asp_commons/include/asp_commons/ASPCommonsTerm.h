#pragma once

#include "asp_commons/ASPQueryType.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace reasoner
{

class ASPCommonsTerm : public std::enable_shared_from_this<ASPCommonsTerm>
{
public:
    ASPCommonsTerm(int lifeTime = 1);
    virtual ~ASPCommonsTerm();
    void addRule(std::string rule);
    std::vector<std::string> getRuleHeads();
    std::vector<std::string> getRuleBodies();
    std::string getProgramSection();
    void setProgramSection(std::string programSection);
    int getLifeTime();
    void setLifeTime(int lifeTime);
    std::vector<std::string> getRules();
    void addFact(std::string fact);
    std::vector<std::string> getFacts();
    void setExternals(std::shared_ptr<std::map<std::string, bool>> externals);
    std::shared_ptr<std::map<std::string, bool>> getExternals();
    std::string getNumberOfModels();
    void setNumberOfModels(std::string numberOfModels);
    ASPQueryType getType();
    void setType(ASPQueryType type);
    long getId();
    void setId(long id);
    int getQueryId();
    void setQueryId(int queryId);
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
    int queryId;
    int lifeTime;
    std::vector<std::string> facts;
    std::shared_ptr<std::map<std::string, bool>> externals;
    ASPQueryType type;
};

} /* namespace reasoner */

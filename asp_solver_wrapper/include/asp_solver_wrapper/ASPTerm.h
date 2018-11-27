#pragma once

#include <alica_solver_interface/SolverTerm.h>
#include <asp_commons/ASPQueryType.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace alica
{
namespace reasoner
{

class ASPTerm : public std::enable_shared_from_this<ASPTerm>, public alica::SolverTerm
{
public:
    ASPTerm(int lifeTime = 1);
    virtual ~ASPTerm();
    void addRule(std::string rule);
    std::vector<std::string> getRuleHeads();
    std::vector<std::string> getRuleBodies();
    std::string getProgramSection();
    void setProgramSection(std::string programSection);
    int getLifeTime();
    std::vector<std::string> getRules();
    void addFact(std::string fact);
    std::vector<std::string> getFacts();
    void setExternals(std::shared_ptr<std::map<std::string, bool>> externals);
    std::shared_ptr<std::map<std::string, bool>> getExternals();
    std::string getNumberOfModels();
    void setNumberOfModels(std::string numberOfModels);
    ::reasoner::ASPQueryType getType();
    void setType(::reasoner::ASPQueryType type);
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
    ::reasoner::ASPQueryType type;
};

} /* namespace reasoner */
} /* namespace alica */

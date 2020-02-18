#pragma once

#include "reasoner/asp/QueryType.h"

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
    virtual ~Term() = default;
    void addRule(const std::string& rule);
    std::vector<std::string> getRuleHeads();
    std::vector<std::string> getRuleBodies();
    std::string getBackgroundKnowledgeFilename();
    void setBackgroundKnowledgeFilename(std::string newBackgroundKnowledgeFilename);
    std::string getBackgroundKnowledgeProgramSection();
    void setBackgroundKnowledgeProgramSection(std::string newBackgroundKnowledgeProgramSection);
    std::vector<std::pair<std::string,std::string>> getProgramSectionParameters();
    void addProgramSectionParameter(const std::string& representation, const std::string& param);

    int getLifeTime();
    void setLifeTime(int newLifeTime);
    std::vector<std::string> getRules();
    void addFact(const std::string& fact); /**< Fact needs to end with a '.'! */
    std::vector<std::string> getFacts();
    void setExternals(std::shared_ptr<std::map<std::string, bool>> newExternals);
    std::shared_ptr<std::map<std::string, bool>> getExternals();
    std::string getNumberOfModels();
    void setNumberOfModels(std::string newNumberOfModels);
    QueryType getType();
    void setType(QueryType newType);
    long getId();
    void setId(long newId);
    std::string getQueryRule();
    void setQueryRule(std::string newQueryRule);

private:
    long id;
    int lifeTime;
    QueryType type;
    std::string backgroundKnowledgeProgramSection;
    std::string backgroundKnowledgeFilename;
    std::string numberOfModels;
    std::string queryRule; /**< The head predicates of this rule are the ones to search for in returned models.*/
    std::vector<std::string> rules;
    std::vector<std::string> heads;
    std::vector<std::string> bodies;

    /**
     * Facts are used as queried values in case of a FilterQuery.
     * In ExtensionQueries, they are simply add to the knowledge base, guarded by the query external.
     */
    std::vector<std::string> facts;

    /**
     * pair first: param representation (e.g. 'n'), second: value (e.g. 1)
     * should be ordered by their occurrence in the program section (#program test(n,m).)
     * Is used for background knowledge program section, as well as for
     * extension query program section (depending on query type).
     */
    std::vector<std::pair<std::string,std::string>> programSectionParameters;

    std::shared_ptr<std::map<std::string, bool>> externals;
};

} /* namespace asp */
} /* namespace reasoner */

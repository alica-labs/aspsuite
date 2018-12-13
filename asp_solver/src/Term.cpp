#include "reasoner/asp/Term.h"

#include <SystemConfig.h>

namespace reasoner
{
namespace asp
{

Term::Term(int lifeTime)
{
    this->programSection = "";
    this->id = -1;
    this->lifeTime = lifeTime;
    this->externals = nullptr;
    this->numberOfModels = "";
    this->type = QueryType::Undefined;
    this->queryId = -1;
}

Term::~Term()
{
}

void Term::addRule(std::string rule)
{
    if (!rule.empty()) {
        this->rules.push_back(rule);
    }
}

void Term::addFact(std::string fact)
{
    if (fact.empty()) {
        return;
    }
    if (fact.find(".") == std::string::npos) {
        this->facts.push_back(fact + ".");
    } else {
        this->facts.push_back(fact);
    }
}

std::vector<std::string> Term::getRuleHeads()
{
    return this->heads;
}

std::vector<std::string> Term::getRuleBodies()
{
    return this->bodies;
}

std::string Term::getProgramSection()
{
    return this->programSection;
}

void Term::setProgramSection(std::string programSection)
{
    this->programSection = programSection;
}

std::string Term::getBackgroundKnowledgeFilename()
{
    return this->backgroundKnowledgeFilename;
}

void Term::setBackgroundKnowledgeFilename(std::string backgroundKnowledgeFilename)
{
    this->backgroundKnowledgeFilename = backgroundKnowledgeFilename;
}

std::vector<std::string> Term::getProgramSectionParameters()
{
    return this->programSectionParameters;
}

void Term::addProgramSectionParameter(std::string param)
{
    this->programSectionParameters.push_back(param);
}

int Term::getLifeTime()
{
    return this->lifeTime;
}

void Term::setLifeTime(int lifeTime)
{
    this->lifeTime = lifeTime;
}

std::vector<std::string> Term::getRules()
{
    return this->rules;
}

std::vector<std::string> Term::getFacts()
{
    return this->facts;
}

void Term::setExternals(std::shared_ptr<std::map<std::string, bool>> externals)
{
    this->externals = externals;
}

std::shared_ptr<std::map<std::string, bool>> Term::getExternals()
{
    return this->externals;
}

std::string Term::getNumberOfModels()
{
    return this->numberOfModels;
}

void Term::setNumberOfModels(std::string numberOfModels)
{
    this->numberOfModels = numberOfModels;
}

QueryType Term::getType()
{
    return this->type;
}

void Term::setType(QueryType type)
{
    this->type = type;
}

long Term::getId()
{
    return id;
}

void Term::setId(long id)
{
    this->id = id;
}

/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the Solver!
 */
int Term::getQueryId()
{
    return queryId;
}

/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the Solver!
 */
void Term::setQueryId(int queryId)
{
    this->queryId = queryId;
}

std::string Term::getQueryRule()
{
    return queryRule;
}

void Term::setQueryRule(std::string queryRule)
{
    if (queryRule.empty()) {
        return;
    }

    queryRule = supplementary::Configuration::trim(queryRule);
    size_t endOfHead = queryRule.find(":-");
    if (endOfHead != std::string::npos) {
        // for rules (including variables)
        size_t startOfBody = endOfHead + 2;
        this->heads.push_back(supplementary::Configuration::trim(queryRule.substr(0, endOfHead)));
        this->bodies.push_back(supplementary::Configuration::trim(queryRule.substr(startOfBody, queryRule.size() - startOfBody - 1)));
    } else {
        // for ground literals
        this->heads.push_back(queryRule);
    }
    this->queryRule = queryRule;
}
} /* namespace asp */
} /* namespace reasoner */

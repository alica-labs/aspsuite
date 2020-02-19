#include "reasoner/asp/Term.h"

#include <utility>

namespace reasoner
{
namespace asp
{

Term::Term(int lifeTime)
        : backgroundKnowledgeProgramSection("")
        , id(-1)
        , lifeTime(lifeTime)
        , externals(nullptr)
        , numberOfModels("")
        , type(QueryType::Undefined)
{
}

void Term::addRule(const std::string& rule)
{
    if (!rule.empty()) {
        this->rules.push_back(rule);
    }
}

void Term::addFact(const std::string& fact)
{
    if (fact.empty()) {
        return;
    }
    this->facts.push_back(fact);
}

//std::vector<std::string> Term::getRuleHeads()
//{
//    return this->heads;
//}
//
//std::vector<std::string> Term::getRuleBodies()
//{
//    return this->bodies;
//}

std::string Term::getBackgroundKnowledgeProgramSection()
{
    return this->backgroundKnowledgeProgramSection;
}

void Term::setBackgroundKnowledgeProgramSection(std::string newBackgroundKnowledgeProgramSection)
{
    this->backgroundKnowledgeProgramSection = std::move(newBackgroundKnowledgeProgramSection);
}

std::string Term::getBackgroundKnowledgeFilename()
{
    return this->backgroundKnowledgeFilename;
}

void Term::setBackgroundKnowledgeFilename(std::string newBackgroundKnowledgeFilename)
{
    this->backgroundKnowledgeFilename = std::move(newBackgroundKnowledgeFilename);
}

std::vector<std::pair<std::string, std::string>> Term::getProgramSectionParameters()
{
    return this->programSectionParameters;
}

void Term::addProgramSectionParameter(const std::string& representation, const std::string& value)
{
    this->programSectionParameters.emplace_back(representation, value);
}

int Term::getLifeTime()
{
    return this->lifeTime;
}

void Term::setLifeTime(int newLifeTime)
{
    this->lifeTime = newLifeTime;
}

std::vector<std::string> Term::getRules()
{
    return this->rules;
}

std::vector<std::string> Term::getFacts()
{
    return this->facts;
}

void Term::setExternals(std::shared_ptr<std::map<std::string, bool>> newExternals)
{
    this->externals = std::move(newExternals);
}

std::shared_ptr<std::map<std::string, bool>> Term::getExternals()
{
    return this->externals;
}

std::string Term::getNumberOfModels()
{
    return this->numberOfModels;
}

void Term::setNumberOfModels(std::string newNumberOfModels)
{
    this->numberOfModels = std::move(newNumberOfModels);
}

QueryType Term::getType()
{
    return this->type;
}

void Term::setType(QueryType newType)
{
    this->type = newType;
}

long Term::getId()
{
    return id;
}

void Term::setId(long newId)
{
    this->id = newId;
}

const std::vector<std::string>& Term::getQueryValues()
{
    return this->queryValues;
}

void Term::addQueryValue(const std::string& queryValue)
{
    this->queryValues.push_back(queryValue);
}

//std::string Term::getQueryRule()
//{
//    return queryRule;
//}

//void Term::setQueryRule(std::string newQueryRule)
//{
//    if (newQueryRule.empty()) {
//        return;
//    }
//
//    newQueryRule = essentials::Configuration::trim(newQueryRule);
//    size_t endOfHead = newQueryRule.find(":-");
//    if (endOfHead != std::string::npos) {
//        // for rules (including variables)
//        size_t startOfBody = endOfHead + 2;
//        this->heads.push_back(essentials::Configuration::trim(newQueryRule.substr(0, endOfHead)));
//        this->bodies.push_back(essentials::Configuration::trim(newQueryRule.substr(startOfBody, newQueryRule.size() - startOfBody - 1)));
//    } else {
//        // for ground literals
//        this->heads.push_back(newQueryRule);
//    }
//    this->queryRule = newQueryRule;
//}
} /* namespace asp */
} /* namespace reasoner */

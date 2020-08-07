/*
 * Term.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <essentials/SystemConfig.h>
#include <asp_solver_wrapper/ASPTerm.h>

namespace alica
{
namespace reasoner
{

ASPTerm::ASPTerm(int lifeTime)
{
    this->programSection = "";
    this->id = -1;
    this->lifeTime = lifeTime;
    this->externals = nullptr;
    this->numberOfModels = "";
    this->type = ::reasoner::asp::QueryType::Undefined;
    this->queryId = -1;
}

ASPTerm::~ASPTerm() {}

void ASPTerm::addRule(std::string rule)
{
    this->rules.push_back(rule);
}

void ASPTerm::addFact(std::string fact)
{
    if (fact.find(".") == std::string::npos) {
        this->facts.push_back(fact + ".");
    } else {
        this->facts.push_back(fact);
    }
}

std::vector<std::string> ASPTerm::getRuleHeads()
{
    return this->heads;
}

std::vector<std::string> ASPTerm::getRuleBodies()
{
    return this->bodies;
}

std::string ASPTerm::getProgramSection()
{
    return this->programSection;
}

void ASPTerm::setProgramSection(std::string programSection)
{
    this->programSection = programSection;
}

int ASPTerm::getLifeTime()
{
    return this->lifeTime;
}

std::vector<std::string> ASPTerm::getRules()
{
    return this->rules;
}

std::vector<std::string> ASPTerm::getFacts()
{
    return this->facts;
}

void ASPTerm::setExternals(std::shared_ptr<std::map<std::string, bool>> externals)
{
    this->externals = externals;
}

std::shared_ptr<std::map<std::string, bool>> ASPTerm::getExternals()
{
    return this->externals;
}

std::string ASPTerm::getNumberOfModels()
{
    return this->numberOfModels;
}

void ASPTerm::setNumberOfModels(std::string numberOfModels)
{
    this->numberOfModels = numberOfModels;
}

::reasoner::asp::QueryType ASPTerm::getType()
{
    return this->type;
}

void ASPTerm::setType(::reasoner::asp::QueryType type)
{
    this->type = type;
}

long ASPTerm::getId()
{
    return id;
}

void ASPTerm::setId(long id)
{
    this->id = id;
}
/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the ASPSolver!
 */
int ASPTerm::getQueryId()
{
    return queryId;
}

/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the ASPSolver!
 */
void ASPTerm::setQueryId(int queryId)
{
    this->queryId = queryId;
}

std::string ASPTerm::getQueryRule()
{
    return queryRule;
}

void ASPTerm::setQueryRule(std::string queryRule)
{
    if (queryRule.empty()) {
        return;
    }

    queryRule = essentials::Configuration::trim(queryRule);
    size_t endOfHead = queryRule.find(":-");
    if (endOfHead != std::string::npos) {
        // for rules (including variables)
        size_t startOfBody = endOfHead + 2;
        this->heads.push_back(essentials::Configuration::trim(queryRule.substr(0, endOfHead)));
        this->bodies.push_back(essentials::Configuration::trim(queryRule.substr(startOfBody, queryRule.size() - startOfBody - 1)));
    } else {
        // for ground literals
        this->heads.push_back(queryRule);
    }
    this->queryRule = queryRule;
}

} /* namespace reasoner */
} /* namespace alica */

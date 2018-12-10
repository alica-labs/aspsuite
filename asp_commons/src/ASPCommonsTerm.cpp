#include <ASPCommonsTerm.h>
#include <SystemConfig.h>

namespace reasoner
{

ASPCommonsTerm::ASPCommonsTerm(int lifeTime)
{
    this->programSection = "";
    this->id = -1;
    this->lifeTime = lifeTime;
    this->externals = nullptr;
    this->numberOfModels = "";
    this->type = ASPQueryType::Undefined;
    this->queryId = -1;
}

ASPCommonsTerm::~ASPCommonsTerm() {}

void ASPCommonsTerm::addRule(std::string rule)
{
    if(!rule.empty()) {
        this->rules.push_back(rule);
    }
}

void ASPCommonsTerm::addFact(std::string fact)
{
    if(fact.empty()) {
        return;
    }
    if (fact.find(".") == std::string::npos) {
        this->facts.push_back(fact + ".");
    } else {
        this->facts.push_back(fact);
    }
}

std::vector<std::string> ASPCommonsTerm::getRuleHeads()
{
    return this->heads;
}

std::vector<std::string> ASPCommonsTerm::getRuleBodies()
{
    return this->bodies;
}

std::string ASPCommonsTerm::getProgramSection()
{
    return this->programSection;
}

void ASPCommonsTerm::setProgramSection(std::string programSection)
{
    this->programSection = programSection;
}

std::vector<std::string> ASPCommonsTerm::getProgramSectionParameters()
{
    return this->programSectionParameters;
}

void ASPCommonsTerm::addProgramSectionParameter(std::string param)
{
    this->programSectionParameters.push_back(param);
}

int ASPCommonsTerm::getLifeTime()
{
    return this->lifeTime;
}

void ASPCommonsTerm::setLifeTime(int lifeTime)
{
    this->lifeTime = lifeTime;
}

std::vector<std::string> ASPCommonsTerm::getRules()
{
    return this->rules;
}

std::vector<std::string> ASPCommonsTerm::getFacts()
{
    return this->facts;
}

void ASPCommonsTerm::setExternals(std::shared_ptr<std::map<std::string, bool>> externals)
{
    this->externals = externals;
}

std::shared_ptr<std::map<std::string, bool>> ASPCommonsTerm::getExternals()
{
    return this->externals;
}

std::string ASPCommonsTerm::getNumberOfModels()
{
    return this->numberOfModels;
}

void ASPCommonsTerm::setNumberOfModels(std::string numberOfModels)
{
    this->numberOfModels = numberOfModels;
}

ASPQueryType ASPCommonsTerm::getType()
{
    return this->type;
}

void ASPCommonsTerm::setType(ASPQueryType type)
{
    this->type = type;
}

long ASPCommonsTerm::getId()
{
    return id;
}

void ASPCommonsTerm::setId(long id)
{
    this->id = id;
}
/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the ASPSolver!
 */
int ASPCommonsTerm::getQueryId()
{
    return queryId;
}

/**
 * The query id has to be added to any predicate which is added to the program, naming rule
 * heads and facts!
 * An unique id is given by the ASPSolver!
 */
void ASPCommonsTerm::setQueryId(int queryId)
{
    this->queryId = queryId;
}

std::string ASPCommonsTerm::getQueryRule()
{
    return queryRule;
}

void ASPCommonsTerm::setQueryRule(std::string queryRule)
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

} /* namespace reasoner */

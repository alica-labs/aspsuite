#include "asp_commons/ASPQuery.h"

#include "asp_commons/ASPCommonsTerm.h"
#include "asp_commons/AnnotatedValVec.h"
#include "asp_commons/IASPSolver.h"

#include <sstream>

namespace reasoner
{

ASPQuery::ASPQuery(IASPSolver* solver, reasoner::ASPCommonsTerm* term)
{
    this->type = ASPQueryType::Undefined;
    this->solver = solver;
    this->term = term;
    this->programSection = term->getProgramSection();
    this->lifeTime = term->getLifeTime();
    this->currentModels = std::make_shared<std::vector<Clingo::SymbolVector>>();

    // load background knowledge file only once (it does not ground anything)
    this->solver->loadFileFromConfig(this->backgroundKnowledgeFilename);

    // ground term program section with given params
    if (!term->getProgramSection().empty()) {
        Clingo::SymbolVector paramsVec;
        auto params = this->term->getProgramSectionParameters();
        for (auto param : params) {
            paramsVec.push_back(this->solver->parseValue(param));
        }
        this->solver->ground({{term->getProgramSection().c_str(), paramsVec}}, nullptr);
    }
}

ASPQuery::~ASPQuery() {}

void ASPQuery::reduceLifeTime()
{
    if (this->lifeTime > 0) {
        this->lifeTime--;
    }
}

void ASPQuery::saveHeadValuePair(Clingo::Symbol key, Clingo::Symbol value)
{
    auto entry = this->headValues.find(key);
    if (entry != this->headValues.end()) {
        if (find(entry->second.begin(), entry->second.end(), value) == entry->second.end()) {
            entry->second.push_back(value);
        }
    }
}

bool ASPQuery::checkMatchValues(Clingo::Symbol value1, Clingo::Symbol value2)
{
    if (value2.type() != Clingo::SymbolType::Function) {
        return false;
    }

    if (value1.name() != value2.name()) {
        return false;
    }
    if (value1.arguments().size() != value2.arguments().size()) {
        return false;
    }

    for (uint i = 0; i < value1.arguments().size(); ++i) {
        Clingo::Symbol arg = value1.arguments()[i];
        if (arg.type() == Clingo::SymbolType::Function && std::string(arg.name()) == IASPSolver::WILDCARD_STRING) {
            continue;
        }

        if (arg.type() == Clingo::SymbolType::Function && value2.arguments()[i].type() == Clingo::SymbolType::Function) {
            if (false == checkMatchValues(arg, value2.arguments()[i])) {
                return false;
            }
        } else if (arg != value2.arguments()[i]) {
            return false;
        }
    }
    return true;
}

std::string ASPQuery::toString()
{
    std::stringstream ss;
    ss << "Query:"
       << "\n";
    ss << "\tModels: \n\t\t";
    int counter = 0;
    for (auto model : *this->currentModels) {
        counter++;
        ss << "Number " << counter << ":\n\t\t\t";
        for (auto pred : model) {
            ss << pred << " ";
        }
        ss << "\n";
    }
    ss << "\tQuery will be used " << this->lifeTime << " times again.\n";

    if (this->getType() == ASPQueryType::Filter) {
        ss << "\tQuery is of type Filter.\n";
        ss << "\tFacts:"
           << "\n";
        for (auto value : this->headValues) {
            ss << "\t\t" << value.first << "\n";
        }
        ss << "\tFacts in models:"
           << "\n";
        for (auto value : this->headValues) {
            ss << "\t\tFact: " << value.first << "\n";
            ss << "\t\t\t In Model: ";
            for (auto predicate : value.second) {
                ss << predicate << " ";
            }
            ss << "\n";
        }
    } else if (this->getType() == ASPQueryType::Extension) {
        ss << "\tQuery is of type Extension.\n";
        ss << "\tRuleHeadValues:"
           << "\n";
        for (auto value : this->headValues) {
            ss << "\t\t" << value.first << "\n";
        }
        ss << "\tRuleHeadValues with models:"
           << "\n";
        for (auto value : this->headValues) {
            ss << "\t\tRuleHead: " << value.first << "\n";
            ss << "\t\t\t Grounded in Model: ";
            for (auto predicate : value.second) {
                ss << predicate << " ";
            }
            ss << "\n";
        }
    } else {
        ss << "\tQuery type is undefined!.\n";
    }

    return ss.str();
}

std::string ASPQuery::getProgramSection()
{
    return this->programSection;
}

void ASPQuery::setProgramSection(std::string programSection)
{
    this->programSection = programSection;
}

IASPSolver* ASPQuery::getSolver()
{
    return this->solver;
}

std::shared_ptr<std::vector<Clingo::SymbolVector>> ASPQuery::getCurrentModels()
{
    return this->currentModels;
}

int ASPQuery::getLifeTime()
{
    return this->lifeTime;
}

void ASPQuery::setLifeTime(int lifeTime)
{
    this->lifeTime = lifeTime;
}

std::vector<std::string> ASPQuery::getRules()
{
    return this->rules;
}

std::map<Clingo::Symbol, Clingo::SymbolVector>& ASPQuery::getHeadValues()
{
    return this->headValues;
}

reasoner::ASPCommonsTerm* ASPQuery::getTerm()
{
    return term;
}

ASPQueryType ASPQuery::getType()
{
    return type;
}

} /* namespace reasoner */

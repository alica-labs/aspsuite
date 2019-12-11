#include "reasoner/asp/Query.h"

#include "reasoner/asp/AnnotatedValVec.h"
#include "reasoner/asp/Solver.h"
#include "reasoner/asp/Term.h"

#include <sstream>

namespace reasoner
{
namespace asp
{

Query::Query(int queryID, Solver* solver, Term* term, QueryType type)
        : queryID(queryID)
        , solver(solver)
        , term(term)
        , type(type)
{
    this->programSection = term->getProgramSection();
    this->lifeTime = term->getLifeTime();
    this->currentModels = std::make_shared<std::vector<Clingo::SymbolVector>>();

    // load background knowledge file only once (it does not ground anything)
    if (!this->term->getBackgroundKnowledgeFilename().empty()) {
        this->solver->loadFileFromConfig(this->term->getBackgroundKnowledgeFilename());
    }

    // ground term program section with given params
    if (!term->getProgramSection().empty()) {
        Clingo::SymbolVector paramsVec;
        for (auto param : this->term->getProgramSectionParameters()) {
            paramsVec.push_back(this->solver->parseValue(param.second));

        }
        this->solver->ground({{term->getProgramSection().c_str(), paramsVec}}, nullptr);
    }
}

Query::~Query() {}

void Query::reduceLifeTime()
{
    if (this->lifeTime > 0) {
        this->lifeTime--;
    }
}

void Query::saveHeadValuePair(Clingo::Symbol key, Clingo::Symbol value)
{
    auto entry = this->headValues.find(key);
    if (entry != this->headValues.end()) {
        if (find(entry->second.begin(), entry->second.end(), value) == entry->second.end()) {
            entry->second.push_back(value);
        }
    }
}

bool Query::checkMatchValues(Clingo::Symbol value1, Clingo::Symbol value2)
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
        if (arg.type() == Clingo::SymbolType::Function && std::string(arg.name()) == Solver::WILDCARD_STRING) {
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

std::string Query::toString()
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

    if (this->getType() == QueryType::Filter) {
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
    } else if (this->getType() == QueryType::Extension) {
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

std::string Query::getProgramSection()
{
    return this->programSection;
}

void Query::setProgramSection(std::string programSection)
{
    this->programSection = programSection;
}

Solver* Query::getSolver()
{
    return this->solver;
}

std::shared_ptr<std::vector<Clingo::SymbolVector>> Query::getCurrentModels()
{
    return this->currentModels;
}

int Query::getLifeTime()
{
    return this->lifeTime;
}

void Query::setLifeTime(int lifeTime)
{
    this->lifeTime = lifeTime;
}

std::map<Clingo::Symbol, Clingo::SymbolVector>& Query::getHeadValues()
{
    return this->headValues;
}

Term* Query::getTerm()
{
    return term;
}

QueryType Query::getType()
{
    return type;
}

int Query::getQueryID() const
{
    return this->queryID;
}

} /* namespace asp */
} /* namespace reasoner */

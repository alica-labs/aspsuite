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
    this->lifeTime = term->getLifeTime();
    this->addQueryValues(term->getQueryValues());
}

// CONFIGURATION and STATE STUFF

Term* Query::getTerm()
{
    return term;
}

Solver* Query::getSolver()
{
    return this->solver;
}

int Query::getLifeTime()
{
    return this->lifeTime;
}

void Query::setLifeTime(int newLifeTime)
{
    this->lifeTime = newLifeTime;
}

void Query::reduceLifeTime()
{
    if (this->lifeTime > 0) {
        this->lifeTime--;
    }
}

QueryType Query::getType()
{
    return type;
}

int Query::getQueryID() const
{
    return this->queryID;
}

// RESULT STUFF

void Query::addQueryValues(const std::vector<std::string>& queryVec)
{
    for (const auto& queryString : queryVec) {
        this->queriedValues.push_back(this->solver->parseValue(queryString));
#ifdef ASPQUERY_DEBUG
        std::cout << "[Query] Queried value: " << queryString << std::endl;
#endif
    }
}

void Query::onModel(Clingo::Model& clingoModel)
{
#ifdef ASPQUERY_DEBUG
    std::cout << "[Query] Model received! " << std::endl;
#endif
    Clingo::SymbolVector newModel;
    this->queryResultMappings.emplace_back();
    auto& mapping = this->queryResultMappings.back();
    for (const auto& queriedValue : this->queriedValues) {
        mapping.emplace(queriedValue, Clingo::SymbolVector());
    }
    for (auto& modelSymbol : clingoModel.symbols(clingo_show_type_shown)) {
        for (auto& entry : mapping) {
            if (Query::match(entry.first, modelSymbol)) {
#ifdef ASPQUERY_DEBUG
                std::cout << "[Query] Matched  " << entry.first << " and " << modelSymbol << std::endl;
#endif
                entry.second.push_back(modelSymbol);
            }
        }
        newModel.push_back(modelSymbol);
    }
    this->currentModels.push_back(newModel);
}

void Query::addQueryResultMapping(int modelIdx, Clingo::Symbol key, Clingo::Symbol value)
{
    auto& queryResultMapping = this->queryResultMappings[modelIdx];
    auto entry = queryResultMapping.find(key);
    if (entry != queryResultMapping.end()) {
        if (find(entry->second.begin(), entry->second.end(), value) == entry->second.end()) {
            entry->second.push_back(value);
        }
    }
}

const std::vector<Clingo::SymbolVector>& Query::getCurrentModels()
{
    return this->currentModels;
}

const std::vector<std::map<Clingo::Symbol, Clingo::SymbolVector>>& Query::getQueryResultMappings()
{
    return this->queryResultMappings;
}

// UTILITY FUNCTIONS

bool Query::match(const Clingo::Symbol& value1,const Clingo::Symbol& value2)
{
#ifdef ASPQUERY_DEBUG
//    std::cout << "[Query] Value 1 '" << value1 << "' Value 2 '" << value2 << "'" << std::endl;
#endif
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
            if (!match(arg, value2.arguments()[i])) { // recursive call
                return false;
            }
        } else if (arg != value2.arguments()[i]) {
            return false;
        }
    }
    return true;
}

std::string Query::toString(bool verbose)
{
    std::stringstream ss;
    ss << "[Query]" << std::endl;
    ss << "Type: " << this->type << std::endl;
    ss << "Current lifetime: " << this->lifeTime << std::endl;
    ss << this->currentModels.size() << " Models" << std::endl;

    int modelCounter = 0;
    while (modelCounter < this->currentModels.size()) {
        if (verbose) {
            ss << "Model " << modelCounter << ":" << std::endl;
            for (auto pred : this->currentModels[modelCounter]) {
                ss << pred << " ";
            }
            ss << std::endl;
        }
        auto& queryResultMapping = this->queryResultMappings[modelCounter];
        for (auto& valueMapping : queryResultMapping) {
            ss << "Queried Value: " << valueMapping.first << " Matched Values: ";
            for (auto matchedValue : valueMapping.second) {
                ss << matchedValue << " ";
            }
            ss << std::endl;
        }
        modelCounter++;
    }
    return ss.str();
}

} /* namespace asp */
} /* namespace reasoner */

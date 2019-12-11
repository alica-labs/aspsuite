#include "reasoner/asp/FilterQuery.h"

#include "reasoner/asp/Enums.h"
#include "reasoner/asp/Solver.h"

namespace reasoner
{
namespace asp
{

FilterQuery::FilterQuery(int queryID, Solver* solver, Term* term)
        : Query(queryID, solver, term, QueryType::Filter)
{
    this->addQueryValues(term->getRuleHeads());
    this->currentModels = std::make_shared<std::vector<Clingo::SymbolVector>>();

#ifdef FILTERQUERY_DEBUG
    std::cout << "FilterQuery: Query contains rule: " << std::endl;
    for (auto rule : this->term->getRules()) {
        std::cout << rule << std::endl;
    }

    for (auto fact : this->term->getFacts()) {
        std::cout << "FilterQuery: Query contains fact: " << fact << std::endl;
    }
#endif
}

FilterQuery::~FilterQuery() {}

void FilterQuery::addQueryValues(std::vector<std::string> queryVec)
{
    for (auto queryString : queryVec) {
        if (queryString.compare("") == 0) {
            return;
        }
        // TODO: Fix nested braces and move functionality to central accessible helper class
        if (queryString.find(",") != std::string::npos) {

            auto preds = this->determineHeadPredicates(queryString);
            for (auto pred : preds) {
                std::stringstream currentQuery;
                currentQuery << pred.name;
                if (pred.arity > 0) {
                    currentQuery << "(";
                    currentQuery << pred.parameters;
                    currentQuery << ")";
                }
                auto res = currentQuery.str();

                this->headValues.emplace(this->solver->parseValue(res), std::vector<Clingo::Symbol>());
		start = queryString.find(",", end);
                if (start != std::string::npos) {
                    start += 1;
                }
            }
        } else {
            this->headValues.emplace(this->solver->parseValue(queryString), std::vector<Clingo::Symbol>());
        }
    }
}

bool FilterQuery::factsExistForAtLeastOneModel()
{
    for (auto queryValue : this->headValues) {
        if (queryValue.second.size() > 0) {
            return true;
        }
    }
    return false;
}

bool FilterQuery::factsExistForAllModels()
{
    for (auto queryValue : this->headValues) {
        if (queryValue.second.size() == 0) {
            return false;
        }
    }
    return true;
}

void FilterQuery::removeExternal()
{ // NOOP in case of FilterQuery
}

std::vector<std::pair<Clingo::Symbol, TruthValue>> FilterQuery::getTruthValues()
{
    std::vector<std::pair<Clingo::Symbol, TruthValue>> ret;
    for (auto iter : this->getHeadValues()) {
        if (iter.second.size() == 0) {
            ret.push_back(std::pair<Clingo::Symbol, TruthValue>(iter.first, TruthValue::Unknown));
        } else {
            if (iter.second.at(0).is_positive()) {
                ret.push_back(std::pair<Clingo::Symbol, TruthValue>(iter.first, TruthValue::True));
            } else {
                ret.push_back(std::pair<Clingo::Symbol, TruthValue>(iter.first, TruthValue::False));
            }
        }
    }
    return ret;
}

void FilterQuery::onModel(Clingo::Model& clingoModel)
{
    // Remember model
    Clingo::SymbolVector vec;
    auto tmp = clingoModel.symbols(clingo_show_type_shown);
    for (int i = 0; i < tmp.size(); i++) {
        vec.push_back(tmp[i]);
    }
    this->getCurrentModels()->push_back(vec);

    // Fill mapping from query fact towards model fact
    for (auto value : this->getHeadValues()) {
#ifdef QUERY_DEBUG
        std::cout << "FilterQuery::onModel: " << value.first << std::endl;
#endif
        auto it = ((Solver*) this->solver)
                          ->getSymbolicAtoms()
                          .begin(Clingo::Signature(value.first.name(), value.first.arguments().size(), value.first.is_positive())); // value.first.signature();
        if (it == ((Solver*) this->solver)->getSymbolicAtoms().end()) {
            std::cout << "FilterQuery: Didn't find any suitable domain!" << std::endl;
            continue;
        }

        bool isNewModel = true;
        while (it) {
            if (clingoModel.contains((*it).symbol()) && this->checkMatchValues(Clingo::Symbol(value.first), (*it).symbol())) {

                // TODO hack for accumulating too many models in optimization
                if (isNewModel) {
                    isNewModel = false;
                    auto entry = this->headValues.find(Clingo::Symbol(value.first));
                    if (entry != this->headValues.end()) {
                        entry->second.clear();
                    }
                }
                this->saveHeadValuePair(Clingo::Symbol(value.first), (*it).symbol());
            }
            it++;
        }
    }
}
} /* namespace asp */
} /* namespace reasoner */

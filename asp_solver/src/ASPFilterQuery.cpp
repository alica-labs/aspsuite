#include "asp_solver/ASPFilterQuery.h"

#include "asp_solver/ASPSolver.h"

#include <asp_commons/IASPSolver.h>

namespace reasoner
{

ASPFilterQuery::ASPFilterQuery(ASPSolver* solver, ASPCommonsTerm* term)
        : ASPQuery(solver, term)
{
    this->type = ASPQueryType::Filter;
    this->addQueryValues(term->getRuleHeads());
    this->currentModels = std::make_shared<std::vector<Clingo::SymbolVector>>();

#ifdef ASPSolver_DEBUG
    std::cout << "ASPSolver: Query contains rule: " << std::endl;
    for (auto rule : this->term->getRules()) {
        std::cout << rule << std::endl;
    }

    for (auto fact : this->term->getFacts()) {
        std::cout << "ASPSolver: Query contains fact: " << fact << std::endl;
    }
#endif
}

ASPFilterQuery::~ASPFilterQuery() {}

void ASPFilterQuery::addQueryValues(std::vector<std::string> queryVec)
{
    for (auto queryString : queryVec) {
        if (queryString.compare("") == 0) {
            return;
        }
        // TODO: Fix nested braces and move funtionality to central accessable helper class
        if (queryString.find(",") != std::string::npos) {
            size_t start = 0;
            size_t end = std::string::npos;
            std::string currentQuery = "";
            while (start != std::string::npos) {
                end = queryString.find(")", start);
                if (end == std::string::npos || end == queryString.size()) {
                    break;
                }
                currentQuery = queryString.substr(start, end - start + 1);
                currentQuery = supplementary::Configuration::trim(currentQuery);
                this->headValues.emplace(this->solver->parseValue(currentQuery), std::vector<Clingo::Symbol>());
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

bool ASPFilterQuery::factsExistForAtLeastOneModel()
{
    for (auto queryValue : this->headValues) {
        if (queryValue.second.size() > 0) {
            return true;
        }
    }
    return false;
}

bool ASPFilterQuery::factsExistForAllModels()
{
    for (auto queryValue : this->headValues) {
        if (queryValue.second.size() == 0) {
            return false;
        }
    }
    return true;
}

void ASPFilterQuery::removeExternal()
{ // NOOP in case of ASPFilterQuery
}

std::vector<std::pair<Clingo::Symbol, ASPTruthValue>> ASPFilterQuery::getASPTruthValues()
{
    std::vector<std::pair<Clingo::Symbol, ASPTruthValue>> ret;
    for (auto iter : this->getHeadValues()) {
        if (iter.second.size() == 0) {
            ret.push_back(std::pair<Clingo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::Unknown));
        } else {
            if (iter.second.at(0).is_positive()) {
                ret.push_back(std::pair<Clingo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::True));
            } else {
                ret.push_back(std::pair<Clingo::Symbol, ASPTruthValue>(iter.first, ASPTruthValue::False));
            }
        }
    }
    return ret;
}

void ASPFilterQuery::onModel(Clingo::Model& clingoModel)
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
#ifdef ASPQUERY_DEBUG
        cout << "ASPFilterQuery::onModel: " << value.first << endl;
#endif
        auto it = ((ASPSolver*) this->solver)
                          ->clingo->symbolic_atoms()
                          .begin(Clingo::Signature(value.first.name(), value.first.arguments().size(), value.first.is_positive())); // value.first.signature();
        if (it == ((ASPSolver*) this->solver)->clingo->symbolic_atoms().end()) {
            std::cout << "ASPFilterQuery: Didn't find any suitable domain!" << std::endl;
            continue;
        }

        while (it) {
            if (clingoModel.contains((*it).symbol()) && this->checkMatchValues(Clingo::Symbol(value.first), (*it).symbol())) {
                this->saveHeadValuePair(Clingo::Symbol(value.first), (*it).symbol());
            }
            it++;
        }
    }
}

} /* namespace reasoner */

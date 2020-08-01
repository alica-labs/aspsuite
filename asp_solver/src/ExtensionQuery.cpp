#include "reasoner/asp/ExtensionQuery.h"

#include "reasoner/asp/Solver.h"

#include <algorithm>
#include <regex>

#include <chrono>

//#define QUERY_DEBUG

namespace reasoner
{
namespace asp
{

ExtensionQuery::ExtensionQuery(Solver* solver, Term* term)
        : Query(solver, term)
{
    // HACK for testing incremental query
    if (term->getType() == QueryType::Extension) {

        this->type = QueryType::Extension;
        std::stringstream ss;
        if (term->getQueryId() == -1) {
#ifdef QUERY_DEBUG
            std::cout << "ExtensionQuery: Error please set the queryId and add it to any additional Fact or Rule that is going to be queried! " << std::endl;
#endif
            return;
        }
        ss << "query" << term->getQueryId();
        this->queryProgramSection = ss.str();

#ifdef QUERY_DEBUG
        std::cout << "ExtensionQuery: creating query number " << term->getQueryId() << " and program section " << this->queryProgramSection << std::endl;
#endif
        this->createProgramSection();
        //    // is added manually to recreate eval for LNAI17 paper
        //    auto loaded2 = this->solver->loadFileFromConfig("alicaBackgroundKnowledgeFile");
        //    if (loaded2) {
        //        this->solver->ground({{"alicaBackground", {}}}, nullptr);
        //    }

        Clingo::SymbolVector paramsVec;
        for (auto param : this->term->getProgramSectionParameters()) {
            paramsVec.push_back(this->solver->parseValue(param.second));
        }

        // TODO segfault here
        this->solver->ground({{this->queryProgramSection.c_str(), paramsVec}}, nullptr);
        this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
    }
}

ExtensionQuery::~ExtensionQuery() = default;

void ExtensionQuery::createProgramSection()
{
    std::stringstream ss;
    // hacky - how to distinguish normal loading of bg knowledge??
    if (!term->getProgramSectionParameters().empty() && !term->getRules().empty()) {
        ss << "(";
        for (int i = 0; i < term->getProgramSectionParameters().size(); ++i) {
            ss << term->getProgramSectionParameters().at(i).first;
            if (i != term->getProgramSectionParameters().size() - 1) {
                ss << ", ";
            }
        }
        ss << ")";
    }

    auto wrappedParams = ss.str();
    auto programSection = this->queryProgramSection + wrappedParams;
    this->externalName = "external" + this->queryProgramSection;

    std::stringstream queryProgram;
    queryProgram << "#program " << programSection << ".\n";
    queryProgram << "#external " << externalName << ".\n";

    for (auto& fact : term->getFacts()) {
        queryProgram << expandFactModuleProperty(fact);
        extractHeadPredicates(fact);
    }
    if (!term->getQueryRule().empty()) {
        extractHeadPredicates(term->getQueryRule());
    }
    for (auto& rule : term->getRules()) {
        extractHeadPredicates(rule);
    }

    for (auto& headPredicate : predicatesToAritiesMap) {
        for (auto& arity : headPredicate.second) {
            queryProgram << createKBCapturingRule(headPredicate.first, arity, this->queryProgramSection);
        }
    }
#ifdef QUERY_DEBUG
    std::cout << "INPUT PROGRAM: " << std::endl;
    std::cout << term->getQueryRule() << std::endl;
    for (auto& rule : term->getRules()) {
        std::cout << rule << std::endl;
    }
    for (auto& fact : term->getFacts()) {
        std::cout << fact << std::endl;
    }
    std::cout << std::endl;

    std::cout << "HEAD VALUES: " << std::endl;
    for (auto& predicateArityPair : predicatesToAritiesMap) {
        std::cout << predicateArityPair.first << ": ";
        for (auto& arity : predicateArityPair.second) {
            std::cout << arity << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
#endif

    if (!term->getQueryRule().empty()) {
        queryProgram << expandRuleModuleProperty(term->getQueryRule(), this->queryProgramSection);
    }
    for (auto& rule : term->getRules()) {
        queryProgram << expandRuleModuleProperty(rule, this->queryProgramSection);
    }

#ifdef QUERY_DEBUG
    std::cout << "RESULT: " << std::endl << queryProgram.str() << std::endl;
#endif
    this->solver->add(this->queryProgramSection.c_str(), {}, queryProgram.str().c_str());
    this->external = std::make_shared<Clingo::Symbol>(this->solver->parseValue(this->externalName));
}

void ExtensionQuery::removeExternal()
{
    std::cout << "~~+~~~~~~~~~~~~~~ releasing external of extquery " << this->getTerm()->getId() << std::endl;
    this->solver->releaseExternal(*(this->external));
}

void ExtensionQuery::onModel(Clingo::Model& clingoModel)
{

    Clingo::SymbolVector vec;
    auto tmp = clingoModel.symbols(clingo_show_type_shown);
    for (int i = 0; i < tmp.size(); i++) {
        vec.push_back(tmp[i]);
    }
    this->getCurrentModels()->push_back(vec);
    //	cout << "Query: processing query '" << queryMapPair.first << "'" << endl;

    // determine the domain of the query predicate
    // Lisa: bug/deprecated? nothing seems to get emplaced into headValues in ExtQuery anymore
    for (auto& value : this->headValues) {

        value.second.clear();
#ifdef QUERY_DEBUG
        std::cout << "ExtensionQuery::onModel: " << value.first << std::endl;
#endif
                std::lock_guard<std::mutex> lock(this->solver->clingoModelMtx);
        auto it = ((Solver*) this->solver)
                          ->clingo->symbolic_atoms()
                          .begin(Clingo::Signature(value.first.name(), value.first.arguments().size(), value.first.is_positive())); // value.first.signature();
        if (it == ((Solver*) this->solver)->clingo->symbolic_atoms().end()) {
            std::cout << "ExtensionQuery: Didn't find any suitable domain!" << std::endl;
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

std::string ExtensionQuery::createKBCapturingRule(const std::string& headPredicateName, int arity, const std::string& querySection)
{
    // build replaced fact
    std::stringstream rule;
    rule << querySection << "(" << headPredicateName << SyntaxUtils::createVariableParameters(arity) << ") :- " << headPredicateName
         << SyntaxUtils::createVariableParameters(arity) << ", " << externalName << ".\n";
    //    std::cout << "KB Fact Rule: " << rule.str();
    return rule.str();
}

std::string ExtensionQuery::expandFactModuleProperty(std::string fact)
{
    fact = SyntaxUtils::trim(fact);
    fact = fact.substr(0, fact.size() - 1);
    std::stringstream ss;
    ss << queryProgramSection << "(" << fact << ") :- " << externalName << ".\n";
    return ss.str();
}

std::string ExtensionQuery::expandRuleModuleProperty(const std::string& rule, const std::string& querySection)
{

    std::pair<SyntaxUtils::Separator, size_t> colonPair;
    std::pair<SyntaxUtils::Separator, size_t> semicolonPair;
    std::pair<SyntaxUtils::Separator, size_t> commaPair;
    std::pair<SyntaxUtils::Separator, size_t> result;
    size_t openingBraceIdx = std::string::npos;
    size_t currentIdx = 0;
    size_t endLastPredicateIdx = 0;
    size_t conditionColonIdx = std::string::npos;
    size_t implicationIdx = SyntaxUtils::findImplication(rule);
    size_t openingCurlyBracesIdx = SyntaxUtils::findNextChar(rule, "{", rule.size(), 0);
    if (SyntaxUtils::isMinOrMax(rule, openingCurlyBracesIdx)) {
        openingCurlyBracesIdx = std::string::npos;
    }

    bool done = false;
    std::stringstream mpRule;

    while (!done) {
        colonPair = std::pair<SyntaxUtils::Separator, size_t>(SyntaxUtils::Separator::Colon, SyntaxUtils::findNextChar(rule, ":", rule.size(), currentIdx));
        semicolonPair =
                std::pair<SyntaxUtils::Separator, size_t>(SyntaxUtils::Separator::Semicolon, SyntaxUtils::findNextChar(rule, ";", rule.size(), currentIdx));
        commaPair = std::pair<SyntaxUtils::Separator, size_t>(SyntaxUtils::Separator::Comma, SyntaxUtils::findNextChar(rule, ",", rule.size(), currentIdx));
        result = SyntaxUtils::determineFirstSeparator(SyntaxUtils::determineFirstSeparator(colonPair, semicolonPair), commaPair);
        openingBraceIdx = SyntaxUtils::findNextChar(rule, "(", result.second, currentIdx);
        if (SyntaxUtils::findNextChar(rule, "}", currentIdx, (openingCurlyBracesIdx == std::string::npos ? 0 : openingCurlyBracesIdx)) != std::string::npos) {
            openingCurlyBracesIdx = SyntaxUtils::findNextChar(rule, "{", result.second, currentIdx);
        }

        // handle next predicate
        SyntaxUtils::Predicate predicate;
        switch (result.first) {
        case SyntaxUtils::Semicolon:
        case SyntaxUtils::Comma:
            if (openingBraceIdx != std::string::npos) {
                predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
            } else {
                predicate.name = "";
                predicate.parameterStartIdx = result.second + 1;
                predicate.parameterEndIdx = result.second + 1;
            }
            break;
        case SyntaxUtils::Colon:
            if (result.second != currentIdx) {
                if (openingBraceIdx != std::string::npos) {
                    predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
                } else {
                    // constant before : or :-
                    predicate = SyntaxUtils::extractConstant(rule, rule.find_last_not_of(' ', result.second - 1) + 1);
                }
            } else {
                if (implicationIdx == std::string::npos || (result.second > implicationIdx && openingCurlyBracesIdx == std::string::npos)) {
                    conditionColonIdx = result.second;
                }
                predicate.name = "";
                predicate.parameterStartIdx = result.second + 1;
                predicate.parameterEndIdx = result.second + 1;
            }
            break;
        case SyntaxUtils::None:
            if (openingBraceIdx == std::string::npos) {
                if (currentIdx != std::string::npos && currentIdx > rule.find_last_of('.')) {
                    predicate.name = "";
                    predicate.parameterStartIdx = result.second + 1;
                    predicate.parameterEndIdx = currentIdx;
                } else {
                    // constant found
                    predicate = SyntaxUtils::extractConstant(rule, rule.find_last_of('.'));
                }
            } else {
                // normal predicate found
                predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
            }
            done = true;
            break;
        default:
            std::cerr << "MP: Separator Char not known!" << std::endl;
        }

        // update currentIdx & endLastPredicateIdx while skipping stuff between predicates for currentIdx
        if (!predicate.name.empty()) {
            currentIdx = SyntaxUtils::findNextCharNotOf(rule, " ,;.}=1234567890n", rule.size(), predicate.parameterEndIdx + 1);
            mpRule << rule.substr(endLastPredicateIdx, (predicate.parameterStartIdx - predicate.name.length()) - endLastPredicateIdx);
            endLastPredicateIdx = predicate.parameterEndIdx + 1;
            if (lookUpPredicate(predicate.name, predicate.arity)) {
                // add mp nested predicate
                // FIXME TODO missing functionality for incremental query: distinguish references to older timesteps
                if (implicationIdx >= currentIdx || implicationIdx == 0 || predicate.name.compare("occurs") == 0 || predicate.name.compare("fieldAhead") == 0) {
                    mpRule << this->queryProgramSection << "(" << predicate.name;
                } else {
                    mpRule << querySection << "(" << predicate.name;
                }
                if (predicate.arity != 0) {
                    mpRule << "(" << predicate.parameters << "))";
                } else {
                    mpRule << ")";
                }
            } else {
                mpRule << predicate.name;
                if (predicate.arity != 0) {
                    mpRule << "(" << predicate.parameters << ")";
                }
            }
            if (currentIdx == std::string::npos) {
                if (implicationIdx == 0) {

                    if (openingCurlyBracesIdx != std::string::npos) {
                        mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ", " << externalName << ".";
                    } else {

                        mpRule << ((conditionColonIdx != std::string::npos && rule.find("maximize") == std::string::npos) ? "; " : ", ") << externalName
                               << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ".";
                    }
                } else if (implicationIdx == std::string::npos) {
                    mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << " :- " << externalName << ".";
                } else {
                    if (openingCurlyBracesIdx != std::string::npos) {
                        mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx)
                               << (conditionColonIdx != std::string::npos ? "," : ", ") << externalName << ".";
                    } else {
                        mpRule << (conditionColonIdx != std::string::npos ? "; " : ", ") << externalName
                               << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ".";
                    }
                }
            }
            size_t bracketIdx = rule.find_last_of('[');
            if (done && bracketIdx != std::string::npos && bracketIdx > rule.find_last_of('.')) {
                mpRule << (conditionColonIdx != std::string::npos ? "; " : ", ") << externalName << "." << rule.substr(rule.find_last_of('.') + 1);
            }
        } else {
            currentIdx = SyntaxUtils::findNextCharNotOf(rule, " ,;.}=1234567890n", rule.size(), predicate.parameterEndIdx);
            if (currentIdx != std::string::npos) {
                if (done) {

                    mpRule << (conditionColonIdx != std::string::npos ? "; " : ", ") << externalName << "." << rule.substr(rule.find_last_of('.') + 1);
                } else {

                    mpRule << rule.substr(endLastPredicateIdx, currentIdx - endLastPredicateIdx);
                    endLastPredicateIdx = currentIdx;
                }
            } else {
                mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx);

                mpRule << (conditionColonIdx != std::string::npos ? "; " : ", ") << externalName << ".";
            }
        }

        if (currentIdx == std::string::npos) {
            done = true;
        }
        // reset brace idx
        openingBraceIdx = std::string::npos;
    }
    mpRule << "\n";

    return mpRule.str();
}

} /* namespace asp */
} /* namespace reasoner */
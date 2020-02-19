#include "reasoner/asp/ExtensionQuery.h"

#include "reasoner/asp/Solver.h"

#include <algorithm>
#include <chrono>

//#define ASPQUERY_DEBUG

namespace reasoner
{
namespace asp
{

ExtensionQuery::ExtensionQuery(int queryID, Solver* solver, Term* term)
        : Query(queryID, solver, term, QueryType::Extension)
{
    // add program part
    this->generateQueryProgram();
    this->solver->add(this->queryProgramSection.c_str(), {}, this->queryProgram.c_str());

    // ground program part
    Clingo::SymbolVector paramsVec;
    for (const auto& param : this->term->getProgramSectionParameters()) {
        paramsVec.push_back(this->solver->parseValue(param.second));
    }
    this->solver->ground({{this->queryProgramSection.c_str(), paramsVec}}, nullptr);

    // set query external of program part
    this->external = std::make_shared<Clingo::Symbol>(this->solver->parseValue(this->externalName));
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}

void ExtensionQuery::generateQueryProgram()
{
    std::stringstream ss;
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

    this->queryProgramSection = "query" + std::to_string(this->queryID);
    this->externalName = "external" + this->queryProgramSection;

    std::stringstream queryProgramStream;
    queryProgramStream << "#program " << this->queryProgramSection << ss.str() << ".\n";
    queryProgramStream << "#external " << externalName << ".\n";

    // 1. collect head predicates for encapsulating these for module property
    for (auto& rule : term->getRules()) {
        rememberPredicates(determineHeadPredicates(rule));
    }
    for (auto& fact : term->getFacts()) {
        rememberPredicates(determineHeadPredicates(fact));
    }

#ifdef ASPQUERY_DEBUG
    std::cout << "INPUT PROGRAM: " << std::endl;
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

    // 2. generate knowledge base capturing rules
    for (auto& headPredicate : predicatesToAritiesMap) {
        for (auto& arity : headPredicate.second) {
            queryProgramStream << createKBCapturingRule(headPredicate.first, arity, this->queryProgramSection);
        }
    }

    // 3. taking care of module property after head predicates where collected
    for (auto& fact : term->getFacts()) {
        queryProgramStream << expandFactModuleProperty(fact);
    }
    for (auto& rule : term->getRules()) {
        queryProgramStream << expandRuleModuleProperty(rule, this->queryProgramSection);
    }
    this->queryProgram = queryProgramStream.str();

#ifdef ASPQUERY_DEBUG
    std::cout << "RESULT: " << std::endl << this->queryProgram << std::endl;
#endif
}

void ExtensionQuery::removeExternal()
{
    this->solver->releaseExternal(*(this->external));
}

/**
 * Creates a rule of the form 'query1(headPredicate(..)) :- headPredicate(..).', in order to take
 * knowledge, already existing in the knowledge base, into account.
 * @param headPredicateName
 * @param arity
 * @param querySection
 * @return The capturing rule.
 */
std::string ExtensionQuery::createKBCapturingRule(const std::string& headPredicateName, int arity, const std::string& querySection)
{
    std::stringstream rule;
    rule << querySection << "(" << headPredicateName << SyntaxUtils::createVariableParameters(arity) << ") :- " << headPredicateName
         << SyntaxUtils::createVariableParameters(arity) << ", " << externalName << ".\n";
    return rule.str();
}

/**
 * Expects the given fact to end with a dot!
 * @param fact
 * @return
 */
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
    size_t skipToIdx = std::string::npos;
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
            std::cerr << "expandRuleModuleProperty: Separator Char not known!" << std::endl;
        }

        // update currentIdx & endLastPredicateIdx while skipping stuff between predicates for currentIdx
        if (!predicate.name.empty()) {
            currentIdx = SyntaxUtils::findNextCharNotOf(rule, " ,;.}=1234567890n", rule.size(), predicate.parameterEndIdx + 1);
            mpRule << rule.substr(endLastPredicateIdx, (predicate.parameterStartIdx - predicate.name.length()) - endLastPredicateIdx);
            endLastPredicateIdx = predicate.parameterEndIdx + 1;
            if (lookUpPredicate(predicate.name, predicate.arity)) {
                // add mp nested predicate
                // FIXME TODO missing functionality for incremental query: distinguish references to older timestemps
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
                               << (conditionColonIdx != std::string::npos ? "; " : ", ") << externalName << ".";
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

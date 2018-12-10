#include "asp_solver/ASPExtensionQuery.h"

#include "asp_solver/ASPSolver.h"

#include <asp_commons/IASPSolver.h>

#include <algorithm>
#include <regex>

namespace reasoner
{

ASPExtensionQuery::ASPExtensionQuery(ASPSolver* solver, reasoner::ASPCommonsTerm* term)
        : ASPQuery(solver, term)
{
    this->type = ASPQueryType::Extension;
    std::stringstream ss;
    if (term->getQueryId() == -1) {
#ifdef ASPVARIABLEQUERY_DEBUG
        cout << "ASPExtensionQuery: Error please set the queryId and add it to any additional Fact or Rule that is going to be queried! " << endl;
#endif
        return;
    }
    ss << "query" << term->getQueryId();
    this->queryProgramSection = ss.str();
#ifdef ASPVARIABLEQUERY_DEBUG
    cout << "ASPExtensionQuery: creating query number " << term->getQueryId() << " and program section " << this->queryProgramSection << endl;
#endif
    this->createProgramSection();
    //    // is added manually to recreate eval for LNAI17 paper
    //    auto loaded2 = this->solver->loadFileFromConfig("alicaBackgroundKnowledgeFile");
    //    if (loaded2) {
    //        this->solver->ground({{"alicaBackground", {}}}, nullptr);
    //    }
    this->solver->ground({{this->queryProgramSection.c_str(), {}}}, nullptr);
    this->solver->assignExternal(*(this->external), Clingo::TruthValue::True);
}

ASPExtensionQuery::~ASPExtensionQuery() = default;

void ASPExtensionQuery::createProgramSection()
{
    this->externalName = "external" + this->queryProgramSection;

    std::stringstream queryProgram;
    queryProgram << "#program " << queryProgramSection << ".\n";
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
            queryProgram << createKBCapturingRule(headPredicate.first, arity);
        }
    }
#ifdef ASPVARIABLEQUERY_DEBUG
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

    queryProgram << expandRuleModuleProperty(term->getQueryRule());
    for (auto& rule : term->getRules()) {
        queryProgram << expandRuleModuleProperty(rule);
    }

#ifdef ASPVARIABLEQUERY_DEBUG
    std::cout << "RESULT: " << std::endl << queryProgram.str() << std::endl;
#endif

    this->solver->add(this->queryProgramSection.c_str(), {}, queryProgram.str().c_str());
    this->external = std::make_shared<Clingo::Symbol>(this->solver->parseValue(this->externalName));
}

void ASPExtensionQuery::removeExternal()
{
    this->solver->releaseExternal(*(this->external));
}

ASPQueryType ASPExtensionQuery::getType()
{
    return this->type;
}

void ASPExtensionQuery::onModel(Clingo::Model& clingoModel)
{
    Clingo::SymbolVector vec;
    auto tmp = clingoModel.symbols(clingo_show_type_shown);
    for (int i = 0; i < tmp.size(); i++) {
        vec.push_back(tmp[i]);
    }
    this->getCurrentModels()->push_back(vec);
    //	cout << "ASPQuery: processing query '" << queryMapPair.first << "'" << endl;

    // determine the domain of the query predicate
    for (auto& value : this->headValues) {

        value.second.clear();
#ifdef ASPQUERY_DEBUG
        cout << "ASPExtensionQuery::onModel: " << value.first << endl;
#endif
        auto it = ((ASPSolver*) this->solver)
                          ->clingo->symbolic_atoms()
                          .begin(Clingo::Signature(value.first.name(), value.first.arguments().size(), value.first.is_positive())); // value.first.signature();
        if (it == ((ASPSolver*) this->solver)->clingo->symbolic_atoms().end()) {
            std::cout << "ASPExtensionQuery: Didn't find any suitable domain!" << std::endl;
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

std::string ASPExtensionQuery::trim(const std::string& str)
{
    const std::string& whitespace = " \t";
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return ""; // no content
    }
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

size_t ASPExtensionQuery::findNextChar(const std::string& predicate, const std::string& chars, size_t end, size_t start)
{
    size_t idx = predicate.find_first_of(chars, start);
    return end <= idx ? std::string::npos : idx;
}

size_t ASPExtensionQuery::findNextCharNotOf(const std::string& predicate, const std::string& chars, size_t end, size_t start)
{
    size_t idx = predicate.find_first_not_of(chars, start);
    return end <= idx ? std::string::npos : idx;
}

/**
 * Finds the implication of a given rule. Also handles #minimize and #maximize, as they are converted to
 * a weak constraint like ":~ not goalReachable(id) : goal(id,_,_). [1@2]"
 * @param rule
 * @return Idx of the implication ":-"
 */
size_t ASPExtensionQuery::findImplication(const std::string& rule)
{
    size_t idxs[] = {rule.find(":-"), rule.find(":~"), rule.find("#minimize"), rule.find("#maximize")};
    return *std::min_element(idxs, idxs + 4);
}

void ASPExtensionQuery::rememberPredicate(std::string predicateName, int arity)
{
    predicateName = trim(predicateName);
    size_t dotIndex = predicateName.find_last_of('.');
    if (dotIndex == predicateName.size() - 1) {
        predicateName = predicateName.substr(0, dotIndex);
    }
    if (predicatesToAritiesMap.find(predicateName) == predicatesToAritiesMap.end()) {
        predicatesToAritiesMap.emplace(predicateName, std::unordered_set<int>({arity}));
    } else {
        predicatesToAritiesMap[predicateName].insert(arity);
    }
}

size_t ASPExtensionQuery::findMatchingClosingBrace(const std::string& predicate, size_t openingBraceIdx)
{
    int numOpenBraces = 1;

    size_t currentIdx = openingBraceIdx + 1;

    while (numOpenBraces != 0 && currentIdx < predicate.size()) {
        size_t nextOpeningBrace = predicate.find_first_of('(', currentIdx + 1);
        size_t nextClosingBrace = predicate.find_first_of(')', currentIdx + 1);

        if (nextClosingBrace == std::string::npos) {
            return std::string::npos;
        }

        if (nextOpeningBrace < nextClosingBrace && nextOpeningBrace != std::string::npos) {
            numOpenBraces++;
            currentIdx = nextOpeningBrace;
        } else {
            numOpenBraces--;
            currentIdx = nextClosingBrace;
        }
    }

    // we did go over the length of the predicate, that sucks
    if (currentIdx >= predicate.size()) {
        currentIdx = std::string::npos;
    }

    return currentIdx;
}

ASPExtensionQuery::Predicate ASPExtensionQuery::extractConstant(std::string rule, size_t constantEndIdxPlusOne)
{
    Predicate predicate;
    size_t leftDelimiterIdx = rule.find_last_of(" ,;:", constantEndIdxPlusOne - 1);
    size_t predicateStartIdx = 0;
    if (leftDelimiterIdx != std::string::npos) {
        if (rule[leftDelimiterIdx + 1] == '-') {
            leftDelimiterIdx = leftDelimiterIdx + 2;
        }
        predicateStartIdx = rule.find_first_not_of(" ,;:", leftDelimiterIdx);
    }

    predicate.name = rule.substr(predicateStartIdx, constantEndIdxPlusOne - predicateStartIdx);
    // cut off starting choice stuff and whitespaces...
    size_t start = predicate.name.find_first_not_of(" ,:;{[");
    if (start != std::string::npos) {
        predicate.name = predicate.name.substr(start);
    }

    predicate.parameterStartIdx = constantEndIdxPlusOne;
    predicate.parameterEndIdx = constantEndIdxPlusOne;
    predicate.parameters = "";
    predicate.arity = 0;
    return predicate;
}

ASPExtensionQuery::Predicate ASPExtensionQuery::extractPredicate(std::string rule, size_t parameterStartIdx)
{
    Predicate predicate;
    size_t leftDelimiterIdx = rule.find_last_of(" ,;:", parameterStartIdx);
    size_t predicateStartIdx = 0;
    if (leftDelimiterIdx != std::string::npos) {
        if (rule[leftDelimiterIdx + 1] == '-') {
            leftDelimiterIdx = leftDelimiterIdx + 2;
        }
        predicateStartIdx = rule.find_first_not_of(" ,;:", leftDelimiterIdx);
    }

    predicate.name = rule.substr(predicateStartIdx, parameterStartIdx - predicateStartIdx);
    // cut off starting choice stuff and whitespaces...
    size_t start = predicate.name.find_first_not_of(" ,:;{[");
    if (start != std::string::npos) {
        predicate.name = predicate.name.substr(start);
    }
    if (rule[parameterStartIdx] != '(') {
        std::cerr << "extractPredicate: WTF Constants are everywhere!" << std::endl;
    }

    // normal predicate
    predicate.parameterStartIdx = parameterStartIdx;
    predicate.parameterEndIdx = findMatchingClosingBrace(rule, predicate.parameterStartIdx);
    predicate.parameters = rule.substr(predicate.parameterStartIdx + 1, predicate.parameterEndIdx - predicate.parameterStartIdx - 1);

    int arity = 0;
    size_t currentIdx = 0;
    while (currentIdx < predicate.parameters.size()) {
        // next parameter found
        arity++;

        size_t nextComma = predicate.parameters.find_first_of(',', currentIdx + 1);
        size_t nextOpeningBrace = predicate.parameters.find_first_of('(', currentIdx + 1);

        if (nextOpeningBrace != std::string::npos && nextOpeningBrace < nextComma) {
            // jump over nested fact
            nextComma = predicate.parameters.find_first_of(',', findMatchingClosingBrace(predicate.parameters, nextOpeningBrace) + 1);
        }

        if (nextComma == std::string::npos) {
            break;
        }

        // update currentIdx
        currentIdx = nextComma + 1;
    }
    predicate.arity = arity;

    return predicate;
}

std::string ASPExtensionQuery::createVariableParameters(int arity)
{
    std::stringstream variableParams;
    if (arity != 0) {
        variableParams << "(";
        for (int i = 0; i < arity - 1; i++) {
            variableParams << "X" << i + 1 << ",";
        }
        variableParams << "X" << arity << ")";
        return variableParams.str();
    } else {
        return "";
    }
}

std::string ASPExtensionQuery::createKBCapturingRule(const std::string& headPredicateName, int arity)
{
    // build replaced fact
    std::stringstream rule;
    rule << queryProgramSection << "(" << headPredicateName << createVariableParameters(arity) << ") :- " << headPredicateName
         << createVariableParameters(arity) << ", " << externalName << ".\n";
    //    std::cout << "KB Fact Rule: " << rule.str();
    return rule.str();
}

std::string ASPExtensionQuery::expandFactModuleProperty(std::string fact)
{
    fact = trim(fact);
    fact = fact.substr(0, fact.size() - 1);
    std::stringstream ss;
    ss << queryProgramSection << "(" << fact << ") :- " << externalName << ".\n";
    //    std::cout << "MP Fact Rule: " << ss.str();
    return ss.str();
}

std::pair<ASPExtensionQuery::Separator, size_t> ASPExtensionQuery::determineFirstSeparator(
        std::pair<ASPExtensionQuery::Separator, size_t> a, std::pair<ASPExtensionQuery::Separator, size_t> b)
{
    if (a.second == std::string::npos) {
        if (b.second == std::string::npos) {
            return {Separator::None, std::string::npos};
        } else {
            return b;
        }
    } else {
        if (b.second == std::string::npos) {
            return a;
        } else {
            return a.second < b.second ? a : b;
        }
    }
};

void ASPExtensionQuery::extractHeadPredicates(const std::string& rule)
{
    size_t implicationIdx = findImplication(rule);
    if (implicationIdx == 0) {
        return;
    }

    std::pair<Separator, size_t> colonPair;
    std::pair<Separator, size_t> semicolonPair;
    std::pair<Separator, size_t> commaPair;
    std::pair<Separator, size_t> result;
    size_t openingBraceIdx = std::string::npos;
    size_t skipToIdx = std::string::npos;
    size_t currentIdx = 0;

    bool done = false;

    while (!done) {
        colonPair = std::pair<Separator, size_t>(Separator::Colon, findNextChar(rule, ":", implicationIdx, currentIdx));
        semicolonPair = std::pair<Separator, size_t>(Separator::Semicolon, findNextChar(rule, ";", implicationIdx, currentIdx));
        commaPair = std::pair<Separator, size_t>(Separator::Comma, findNextChar(rule, ",", implicationIdx, currentIdx));
        result = determineFirstSeparator(determineFirstSeparator(colonPair, semicolonPair), commaPair);
        openingBraceIdx = findNextChar(rule, "(", result.second, currentIdx);

        // handle next predicate
        switch (result.first) {
        case Semicolon:
        case Comma:
            if (openingBraceIdx == std::string::npos) {
                // constant
                rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
            } else {
                Predicate predicate = extractPredicate(rule, openingBraceIdx);
                // writes into skipToIdx
                skipToIdx = predicate.parameterEndIdx;
                rememberPredicate(predicate.name, predicate.arity);
            }
            break;
        case Colon:
            // skip behind conditional: ';' or '}' will end conditional
            skipToIdx = findNextChar(rule, "}", implicationIdx, result.second + 1);
            if (skipToIdx == std::string::npos) {
                skipToIdx = findNextChar(rule, ";", implicationIdx, result.second + 1);
            } else {
                skipToIdx = findNextChar(rule, ",;", implicationIdx, skipToIdx + 1);
                if (skipToIdx == std::string::npos) {
                    done = true;
                }
            }
            break;
        case None:
            if (openingBraceIdx == std::string::npos) {
                // constant
                rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
            } else {
                // unary predicate
                rememberPredicate(rule.substr(currentIdx, openingBraceIdx - currentIdx), 1);
            }
            break;
        default:
            std::cerr << "MP: Separator Char not known!" << std::endl;
        }

        // update currentIdx
        if (skipToIdx != std::string::npos) {
            currentIdx = findNextCharNotOf(rule, " ,;.", implicationIdx, skipToIdx + 1);
            if (currentIdx == std::string::npos) {
                done = true;
            }
        } else {
            if (result.second == std::string::npos) {
                done = true;
            }
            currentIdx = result.second + 1;
        }

        // reset brace idx
        openingBraceIdx = std::string::npos;
        skipToIdx = std::string::npos;
    }
}

bool ASPExtensionQuery::lookUpPredicate(const std::string& predicateName, int arity)
{
    if (predicatesToAritiesMap.find(predicateName) != predicatesToAritiesMap.end()) {
        // name matched
        auto& aritiesSet = predicatesToAritiesMap[predicateName];
        return aritiesSet.find(arity) != aritiesSet.end();
    }
    return false;
}

std::string ASPExtensionQuery::expandRuleModuleProperty(const std::string& rule)
{
    std::pair<Separator, size_t> colonPair;
    std::pair<Separator, size_t> semicolonPair;
    std::pair<Separator, size_t> commaPair;
    std::pair<Separator, size_t> result;
    size_t openingBraceIdx = std::string::npos;
    size_t currentIdx = 0;
    size_t endLastPredicateIdx = 0;

    bool done = false;
    std::stringstream mpRule;

    while (!done) {
        colonPair = std::pair<Separator, size_t>(Separator::Colon, findNextChar(rule, ":", rule.size(), currentIdx));
        semicolonPair = std::pair<Separator, size_t>(Separator::Semicolon, findNextChar(rule, ";", rule.size(), currentIdx));
        commaPair = std::pair<Separator, size_t>(Separator::Comma, findNextChar(rule, ",", rule.size(), currentIdx));
        result = determineFirstSeparator(determineFirstSeparator(colonPair, semicolonPair), commaPair);
        openingBraceIdx = findNextChar(rule, "(", result.second, currentIdx);

        // handle next predicate
        Predicate predicate;
        switch (result.first) {
        case Semicolon:
        case Comma:
            if (openingBraceIdx != std::string::npos) {
                predicate = extractPredicate(rule, openingBraceIdx);
            } else {
                predicate.name = "";
                predicate.parameterStartIdx = result.second + 1;
                predicate.parameterEndIdx = result.second + 1;
            }
            break;
        case Colon:
            if (result.second != currentIdx) {
                // constant before : or :-
                predicate = extractConstant(rule, rule.find_last_not_of(' ', result.second - 1) + 1);
            } else {
                predicate.name = "";
                predicate.parameterStartIdx = result.second + 1;
                predicate.parameterEndIdx = result.second + 1;
            }
            break;
        case None:
            if (openingBraceIdx == std::string::npos) {
                if (currentIdx != std::string::npos && currentIdx > rule.find_last_of('.')) {
                    predicate.name = "";
                    predicate.parameterStartIdx = result.second + 1;
                    predicate.parameterEndIdx = currentIdx;
                } else {
                    // constant found (-1 because of . at the end of rules)
                    predicate = extractConstant(rule, rule.size() - 1);
                }
            } else {
                // normal predicate found
                predicate = extractPredicate(rule, openingBraceIdx);
            }
            done = true;
            break;
        default:
            std::cerr << "MP: Separator Char not known!" << std::endl;
        }

        // update currentIdx & endLastPredicateIdx while skipping stuff between predicates for currentIdx
        if (!predicate.name.empty()) {
            currentIdx = findNextCharNotOf(rule, " ,;.}=1234567890", rule.size(), predicate.parameterEndIdx + 1);
            mpRule << rule.substr(endLastPredicateIdx, (predicate.parameterStartIdx - predicate.name.length()) - endLastPredicateIdx);
            endLastPredicateIdx = predicate.parameterEndIdx + 1;
            if (lookUpPredicate(predicate.name, predicate.arity)) {
                // add mp nested predicate
                mpRule << queryProgramSection << "(" << predicate.name << "(" << predicate.parameters << "))";
            } else {
                mpRule << predicate.name;
                if (predicate.arity != 0) {
                    mpRule << "(" << predicate.parameters << ")";
                }
            }
            if (currentIdx == std::string::npos) {
                mpRule << ", " << externalName;
                mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ".";
            }
        } else {
            currentIdx = findNextCharNotOf(rule, " ,;.}=1234567890", rule.size(), predicate.parameterEndIdx);
            if (currentIdx != std::string::npos) {
                if (done) {
                    mpRule << ", " << externalName;
                }
                mpRule << rule.substr(endLastPredicateIdx, currentIdx - endLastPredicateIdx);
                endLastPredicateIdx = currentIdx;
            } else {
                mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ", " << externalName << ".";
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

} /* namespace reasoner */

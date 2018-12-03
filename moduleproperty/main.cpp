#include <bits/stdc++.h>

#include <map>
#include <string>
#include <vector>

static std::string queryProgramSection = "query1";
static std::string externalName = "externalQuery1";
static std::map<std::string, std::unordered_set<int>> predicatesToAritiesMap;

std::string trim(const std::string& str)
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

size_t findNextChar(std::string predicate, char c, int length, int start = 0)
{
    size_t idx = predicate.find_first_of(c, start);
    return length <= idx ? std::string::npos : idx;
}

size_t findNextCharNotOf(std::string predicate, std::string chars, int length, int start = 0)
{
    size_t idx = predicate.find_first_not_of(chars, start);
    return length <= idx ? std::string::npos : idx;
}

size_t findImplication(std::string rule)
{
    return std::max(rule.find_first_of(":-"), rule.find_first_of(":~"));
}

void rememberPredicate(std::string predicateName, int arity)
{
    if (predicatesToAritiesMap.find(predicateName) == predicatesToAritiesMap.end()) {
        predicatesToAritiesMap.emplace(predicateName, std::unordered_set<int>({arity}));
    } else {
        predicatesToAritiesMap[predicateName].insert(arity);
    }

    std::cout << "Remember: " << predicateName << " arity " << arity << std::endl;
}

size_t findMatchingClosingBrace(std::string predicate, int openingBraceIdx)
{
    int numOpenBraces = 1;

    size_t currentIdx = openingBraceIdx + 1;

    while (numOpenBraces != 0 && currentIdx < predicate.size()) {
        int nextOpeningBrace = predicate.find_first_of('(', currentIdx + 1);
        int nextClosingBrace = predicate.find_first_of(')', currentIdx + 1);

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

int determineArity(std::string predicate, size_t posOpeningBrace, size_t& outClosingBraceIdx)
{
    outClosingBraceIdx = findMatchingClosingBrace(predicate, posOpeningBrace);
    std::string factParametersString = predicate.substr(posOpeningBrace + 1, outClosingBraceIdx - posOpeningBrace - 1);

    int arity = 0;

    size_t currentIdx = 0;
    while (currentIdx < factParametersString.size()) {
        // next parameter found
        arity++;

        int nextComma = factParametersString.find_first_of(',', currentIdx + 1);
        int nextOpeningBrace = factParametersString.find_first_of('(', currentIdx + 1);

        if (nextOpeningBrace != std::string::npos && nextOpeningBrace < nextComma) {
            // jump over nested fact
            nextComma = factParametersString.find_first_of(',', findMatchingClosingBrace(factParametersString, nextOpeningBrace) + 1);
        }

        if (nextComma == std::string::npos) {
            break;
        }

        // update currentIdx
        currentIdx = nextComma + 1;
    }

    return arity;
}

std::string replaceAtomsByVariables(std::string fact)
{
    size_t openingBraceIdx = findNextChar(fact, '(', fact.size());
    if (openingBraceIdx == std::string::npos) {
        // constant
        if (fact.find_last_of('.') == fact.size() - 1) {
            // cut of . from fact, if present
            std::string trimmedFact = fact.substr(0, fact.size() - 1);
            rememberPredicate(trimmedFact, 0);
            return trimmedFact;
        } else {
            // no . at the end of fact is present
            rememberPredicate(fact, 0);
            return fact;
        }
    }
    size_t closingBraceIdx = 0;
    int arity = determineArity(fact, openingBraceIdx, closingBraceIdx);

    // build replaced fact
    std::stringstream replacedFact;
    replacedFact << fact.substr(0, openingBraceIdx);
    std::string predicateName = replacedFact.str();
    replacedFact << "(";
    for (int i = 0; i < arity - 1; i++) {
        replacedFact << "X" << i + 1 << ",";
    }
    replacedFact << "X" << arity << ")";

    // remember predicates to encapsulate later
    rememberPredicate(predicateName, arity);

    return replacedFact.str();
}

std::string createKBCapturingFactRule(std::string fact)
{
    std::stringstream additonalFactRule;
    std::string variableFact = replaceAtomsByVariables(fact);
    std::stringstream ss;
    ss << queryProgramSection << "(" << variableFact << ") :- " << variableFact << ", " << externalName << ".\n";
    //    std::cout << "KB Fact Rule: " << ss.str();
    return ss.str();
}

std::string expandFactModuleProperty(std::string fact)
{
    fact = trim(fact);
    fact = fact.substr(0, fact.size() - 1);
    std::stringstream ss;
    ss << queryProgramSection << "(" << fact << ") :- " << externalName << ".\n";
    //    std::cout << "MP Fact Rule: " << ss.str();
    return ss.str();
}

enum Separator
{
    Comma,
    Colon,
    Semicolon,
    None
};

std::pair<Separator, size_t> determineFirst(std::pair<Separator, size_t> a, std::pair<Separator, size_t> b)
{
    if (a.second == std::string::npos) {
        if (b.second == std::string::npos) {
            return std::pair<Separator, size_t>(Separator::None, std::string::npos);
        } else {
            return b;
        }
    } else {
        if (b.second == std::string::npos) {
            return a;
        } else {
            if (a.second < b.second) {
                return a;
            } else {
                return b;
            }
        }
    }
};

void extractHeadPredicates(std::string rule)
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
    size_t closingBraceIdx = std::string::npos;
    size_t currentIdx = 0;

    while (currentIdx != std::string::npos) {
        colonPair = std::pair<Separator, size_t>(Separator::Colon, findNextChar(rule, ':', implicationIdx, currentIdx));
        semicolonPair = std::pair<Separator, size_t>(Separator::Semicolon, findNextChar(rule, ';', implicationIdx, currentIdx));
        commaPair = std::pair<Separator, size_t>(Separator::Comma, findNextChar(rule, ',', implicationIdx, currentIdx));
        result = determineFirst(determineFirst(colonPair, semicolonPair), commaPair);
        openingBraceIdx = findNextChar(rule, '(', result.second, currentIdx);

        // handle next predicate
        switch (result.first) {
        case Comma:
            if (openingBraceIdx == std::string::npos) {
                // constant
                rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
            } else {
                int arity = determineArity(rule, openingBraceIdx, closingBraceIdx);
                rememberPredicate(rule.substr(currentIdx, openingBraceIdx - currentIdx), arity);
            }
            break;
        case Semicolon:
            break;
        case Colon:
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
        if (closingBraceIdx != std::string::npos) {
            currentIdx = findNextCharNotOf(rule, " ,;:", implicationIdx - closingBraceIdx, closingBraceIdx);
            if (currentIdx == std::string::npos) {
                break;
            }
        }
        currentIdx = result.second;

        // reset brace idx
        openingBraceIdx = std::string::npos;
        closingBraceIdx = std::string::npos;
    }
}

int main()
{
    // query rule
    std::string queryRule = "factA(X, Y):-factB(X).";

    // additional rules
    std::vector<std::string> rules;

    // facts
    std::vector<std::string> facts;
    facts.push_back("factA.");
    facts.push_back("factA(1).");
    facts.push_back("factB(1)");
    facts.push_back("factC(2).");
    facts.push_back("factZ(1)");
    facts.push_back("factZ(1251).");
    facts.push_back("factZ(asc, 1251).");
    facts.push_back("factAB(wtf, you, dick, head).");
    facts.push_back("factAB(factB(wtf, you, factZ(1), head)).");

    std::stringstream queryProgram;
    queryProgram << "#program " << queryProgramSection << ".\n";
    queryProgram << "#external " << externalName << ".\n";

    for (auto& fact : facts) {
        queryProgram << expandFactModuleProperty(fact);
        queryProgram << createKBCapturingFactRule(fact);
    }

    extractHeadPredicates(queryRule);
    for (auto& rule : rules) {
        extractHeadPredicates(rule);
    }

    std::cout << "RESULT: " << std::endl << queryProgram.str() << std::endl;

    std::cout << "HEAD VALUES: " << std::endl;
    for (auto& predicateArityPair : predicatesToAritiesMap) {
        std::cout << predicateArityPair.first << ": ";
        for (auto& arity : predicateArityPair.second) {
            std::cout << arity << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}
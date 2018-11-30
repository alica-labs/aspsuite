#include <bits/stdc++.h>

#include <string>
#include <vector>
#include <map>

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

size_t findFirstOpeningBrace(std::string predicate, int start = 0)
{
    return predicate.find_first_of('(', start);
}

size_t findImplication (std::string rule)
{
    return std::max(rule.find_first_of(":-"), rule.find_first_of(":~"));
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

std::string replaceAtomsByVariables(std::string fact)
{
    size_t openingBraceIdx = findFirstOpeningBrace(fact);
    if (openingBraceIdx == std::string::npos) {
        // constant
        if (fact.find_last_of('.') == fact.size()-1) {
            // cut of . from fact, if present
            return fact.substr(0, fact.size() - 1);
        } else {
            // no . at the end of fact is present
            return fact;
        }
    }
    std::string factParametersString = fact.substr(openingBraceIdx + 1, findMatchingClosingBrace(fact, openingBraceIdx) - openingBraceIdx - 1);

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
    if (predicatesToAritiesMap.find(predicateName) != predicatesToAritiesMap.end()) {
        predicatesToAritiesMap.emplace(predicateName, std::unordered_set<int>({arity}));
    } else {
        predicatesToAritiesMap[predicateName].insert(arity);
    }

    return replacedFact.str();
}

std::string createKBCapturingFactRule(std::string fact)
{
    std::stringstream additonalFactRule;
    std::string variableFact = replaceAtomsByVariables(fact);
    std::stringstream ss;
    ss << queryProgramSection << "(" << variableFact << ") :- " << variableFact << ", " << externalName << ".\n";
    std::cout << "KnowledgeBase capturing fact rule: " << ss.str();
    return ss.str();
}

std::string expandFactModuleProperty(std::string fact)
{
    fact = trim(fact);
    fact = fact.substr(0, fact.size() - 1);
    std::stringstream ss;
    ss << queryProgramSection << "(" << fact << ") :- " << externalName << ".\n";
    std::cout << "MP Fact: " << ss.str();
    return ss.str();
}

void extractHeadPredicates (std::string rule) {
    size_t implicationIdx = findImplication(rule);
    if (implicationIdx == 0) {
        return;
    }

    size_t openingBraceIdx = findFirstOpeningBrace(rule);
//    size_t colonIdx = findFirstColon(rule);
//    size_t semicolonIdx = findFirstSemicolon(rule);








    size_t matchingBraceIdx = findMatchingClosingBrace(rule, openingBraceIdx);
}

int main()
{
    // query rule
    std::string queryRule = "factA(X):-factB(X).";

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

    return 0;
}
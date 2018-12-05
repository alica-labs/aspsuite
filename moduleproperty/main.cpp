#include <bits/stdc++.h>

#include <map>
#include <string>
#include <vector>

static std::string queryProgramSection = "query1";
static std::string externalName = "externalQuery1";
static std::map<std::string, std::unordered_set<int>> predicatesToAritiesMap;

struct Predicate
{
    std::string name;
    std::string parameters;
    int arity;
    size_t parameterStartIdx;
    size_t parameterEndIdx;

    Predicate()
            : name("")
            , parameters("")
            , arity(0)
            , parameterStartIdx(0)
            , parameterEndIdx(0)
    {
    }
};

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

size_t findNextChar(std::string predicate, std::string chars, int end, int start = 0)
{
    size_t idx = predicate.find_first_of(chars, start);
    return end <= idx ? std::string::npos : idx;
}

size_t findNextCharNotOf(std::string predicate, std::string chars, int end, int start = 0)
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
size_t findImplication(std::string rule)
{
    size_t idxs[] = {rule.find(":-"), rule.find(":~"), rule.find("#minimize"), rule.find("#maximize")};
    return *std::min_element(idxs, idxs + 4);
}

void rememberPredicate(std::string predicateName, int arity)
{
    if (predicatesToAritiesMap.find(predicateName) == predicatesToAritiesMap.end()) {
        predicatesToAritiesMap.emplace(predicateName, std::unordered_set<int>({arity}));
    } else {
        predicatesToAritiesMap[predicateName].insert(arity);
    }
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

// size_t rfind_first_of(std::string rule, size_t start, const char* chars, int charCount)
//{
//    size_t idx = 0;
//    for (int i = 0; i < charCount; i++) {
//        size_t foundIdx = rule.rfind(chars[i], start);
//        if (foundIdx != std::string::npos && foundIdx > idx) {
//            idx = foundIdx + 1;
//        }
//    }
//    return idx;
//}

Predicate extractPredicate(std::string rule, size_t parameterStartIdx)
{
    Predicate predicate;
    size_t leftDelimiterIdx = rule.find_last_of(" ,;:", 0, parameterStartIdx);
    size_t predicateStartIdx = 0;
    if (leftDelimiterIdx != std::string::npos) {
        predicateStartIdx = rule.find_first_not_of(" ,;:", leftDelimiterIdx);
    }

    predicate.name = rule.substr(predicateStartIdx, parameterStartIdx - predicateStartIdx);
    // cut off starting choice stuff and whitespaces...
    size_t start = predicate.name.find_first_not_of(" ,:;{[");
    if (start != std::string::npos) {
        predicate.name = predicate.name.substr(start);
    }
    if (rule[parameterStartIdx] != '(') {
        // constant
        predicate.parameterStartIdx = parameterStartIdx;
        predicate.parameterEndIdx = parameterStartIdx;
        predicate.parameters = "";
        predicate.arity = 0;
        return predicate;
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

        int nextComma = predicate.parameters.find_first_of(',', currentIdx + 1);
        int nextOpeningBrace = predicate.parameters.find_first_of('(', currentIdx + 1);

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

std::string replaceAtomsByVariables(std::string fact)
{
    size_t openingBraceIdx = findNextChar(fact, "(", fact.size());
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
    Predicate predicate = extractPredicate(fact, openingBraceIdx);

    // build replaced fact
    std::stringstream replacedFact;
    replacedFact << predicate.name;
    replacedFact << "(";
    for (int i = 0; i < predicate.arity - 1; i++) {
        replacedFact << "X" << i + 1 << ",";
    }
    replacedFact << "X" << predicate.arity << ")";

    // remember predicates to encapsulate later
    rememberPredicate(predicate.name, predicate.arity);

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
    size_t skipToIdx = std::string::npos;
    size_t currentIdx = 0;

    bool done = false;

    while (!done) {
        colonPair = std::pair<Separator, size_t>(Separator::Colon, findNextChar(rule, ":", implicationIdx, currentIdx));
        semicolonPair = std::pair<Separator, size_t>(Separator::Semicolon, findNextChar(rule, ";", implicationIdx, currentIdx));
        commaPair = std::pair<Separator, size_t>(Separator::Comma, findNextChar(rule, ",", implicationIdx, currentIdx));
        result = determineFirst(determineFirst(colonPair, semicolonPair), commaPair);
        openingBraceIdx = findNextChar(rule, "(", result.second, currentIdx);

        // handle next predicate
        switch (result.first) {
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
        case Semicolon:
            std::cerr << "MP: Semicolon in head not implemented, yet! If you read this, please create ticket on github with an example rule. THX" << std::endl;
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
            currentIdx = findNextCharNotOf(rule, " ,;", implicationIdx, skipToIdx + 1);
            if (currentIdx == std::string::npos) {
                done = true;
            }
        } else {
            currentIdx = result.second + 1;
        }

        // reset brace idx
        openingBraceIdx = std::string::npos;
        skipToIdx = std::string::npos;
    }
}

bool lookUpPredicate(std::string predicateName, int arity)
{
    if (predicatesToAritiesMap.find(predicateName) != predicatesToAritiesMap.end()) {
        // name matched
        auto& aritiesSet = predicatesToAritiesMap[predicateName];
        if (aritiesSet.find(arity) != aritiesSet.end()) {
            return true;
        }
    }
    return false;
}

std::string expandRuleModuleProperty(std::string rule)
{
    std::pair<Separator, size_t> colonPair;
    std::pair<Separator, size_t> semicolonPair;
    std::pair<Separator, size_t> commaPair;
    std::pair<Separator, size_t> result;
    size_t openingBraceIdx = std::string::npos;
    size_t skipToIdx = std::string::npos;
    size_t currentIdx = 0;
    size_t endLastPredicateIdx = 0;

    bool done = false;
    std::stringstream mpRule;

    while (!done) {
        colonPair = std::pair<Separator, size_t>(Separator::Colon, findNextChar(rule, ":", rule.size(), currentIdx));
        semicolonPair = std::pair<Separator, size_t>(Separator::Semicolon, findNextChar(rule, ";", rule.size(), currentIdx));
        commaPair = std::pair<Separator, size_t>(Separator::Comma, findNextChar(rule, ",", rule.size(), currentIdx));
        result = determineFirst(determineFirst(colonPair, semicolonPair), commaPair);
        openingBraceIdx = findNextChar(rule, "(", result.second, currentIdx);

        // handle next predicate
        Predicate predicate;
        switch (result.first) {
        case Comma:
            predicate = extractPredicate(rule, openingBraceIdx);
            break;
        case Semicolon:

            break;
        case Colon:
            if (result.second != currentIdx) {
                // constant before : or :-
                predicate = extractPredicate(rule, rule.find_last_not_of(" ", currentIdx, result.second - currentIdx));
            } else {
                predicate.name = "";
                predicate.parameterStartIdx = result.second + 1;
                predicate.parameterEndIdx = result.second + 1;
            }
            break;
        case None:
            // constant found
            predicate = extractPredicate(rule, rule.size());
            done = true;
            break;
        default:
            std::cerr << "MP: Separator Char not known!" << std::endl;
        }

        // add middle stuff like commas, 1=, { etc.
        if (predicate.name != "") {
            mpRule << rule.substr(endLastPredicateIdx, predicate.parameterStartIdx - predicate.name.length());
        }

        if (predicate.name != "") {
            if (lookUpPredicate(predicate.name, predicate.arity)) {
                // add mp nested predicate
                mpRule << queryProgramSection << "(" << predicate.name << "(" << predicate.parameters << "))";
            } else {
                mpRule << predicate.name << "(" << predicate.parameters << ")";
            }
        }

        // update currentIdx & endLastPredicateIdx while skipping stuff between predicates for currentIdx
        currentIdx = findNextCharNotOf(rule, " ,;", rule.size(), predicate.parameterEndIdx + 1);
        if (currentIdx == std::string::npos) {
            done = true;
        }
        if (predicate.name != "") {
            endLastPredicateIdx = predicate.parameterEndIdx + 1;
        }

        // reset brace idx
        openingBraceIdx = std::string::npos;
    }
    mpRule << ".\n";
    std::string tmpString = mpRule.str();
    return tmpString;
}

int main()
{
    // query rule
    std::string queryRule = "factA(X, Y):-factB(X).";

    // additional rules
    std::vector<std::string> rules;
    rules.push_back("timestep(0..X,id) :- maxTimestep(id,X).");
    rules.push_back("{goal(id,X,Y) : field(X,Y), not visited(X,Y)} = 1 :- not haveGold.");
    rules.push_back("{maxTimestep(id,0..(N*N)) : fieldSize(N)} = 1.");
    rules.push_back(":~ not goalReachable(id) : goal(id,_,_). [1@2]");
    rules.push_back("#minimize {T@1,id : maxTimestep(id,T)}.");
    rules.push_back("fieldAhead(X-1,Y,T,id) :- field(X,Y), field(X-1,Y), holds(heading(0),T,id), holds(on(X,Y),T,id), timestep(T,id).");

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

    std::cout << "INPUT PROGRAM: " << std::endl;
    std::cout << queryRule << std::endl;
    for (auto& rule : rules) {
        std::cout << rule << std::endl;
    }
    for (auto& fact : facts) {
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

    queryProgram << expandRuleModuleProperty(queryRule);
    for (auto& rule : rules) {
        queryProgram << expandRuleModuleProperty(rule);
    }

    std::cout << "RESULT: " << std::endl << queryProgram.str() << std::endl;

    return 0;
}
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

size_t findNextChar(const std::string& predicate, const std::string& chars, size_t end, size_t start = 0)
{
    size_t idx = predicate.find_first_of(chars, start);
    return end <= idx ? std::string::npos : idx;
}

size_t findNextCharNotOf(const std::string& predicate, const std::string& chars, size_t end, size_t start = 0)
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
size_t findImplication(const std::string& rule)
{
    size_t idxs[] = {rule.find(":-"), rule.find(":~"), rule.find("#minimize"), rule.find("#maximize")};
    return *std::min_element(idxs, idxs + 4);
}

void rememberPredicate(std::string predicateName, int arity)
{
    if (predicateName.empty()) {
        return;
    }
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

size_t findMatchingClosingBrace(const std::string& predicate, size_t openingBraceIdx)
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

Predicate extractConstant(std::string rule, size_t constantEndIdxPlusOne)
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

Predicate extractPredicate(std::string rule, size_t parameterStartIdx)
{
    Predicate predicate;
    size_t leftDelimiterIdx = rule.find_last_of(" ,;:", parameterStartIdx);
    size_t predicateStartIdx = 0;
    if (leftDelimiterIdx != std::string::npos) {
        if (rule[leftDelimiterIdx + 1] == '-') {
            leftDelimiterIdx = leftDelimiterIdx + 2;
        }
        predicateStartIdx = rule.find_first_not_of(" ,;:", leftDelimiterIdx);
    } else {
        predicateStartIdx = rule.find_first_not_of(" ,;:{=0123456789");
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

std::string createVariableParameters(int arity)
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

std::string createKBCapturingRule(const std::string& headPredicateName, int arity)
{
    // build replaced fact
    std::stringstream rule;
    rule << queryProgramSection << "(" << headPredicateName << createVariableParameters(arity) << ") :- " << headPredicateName
         << createVariableParameters(arity) << ", " << externalName << ".\n";
    //    std::cout << "KB Fact Rule: " << rule.str();
    return rule.str();
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

std::pair<Separator, size_t> determineFirstSeparator(std::pair<Separator, size_t> a, std::pair<Separator, size_t> b)
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

void extractHeadPredicates(const std::string& rule)
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
            if (openingBraceIdx == std::string::npos) {
                // constant
                rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
            } else {
                Predicate predicate = extractPredicate(rule, openingBraceIdx);
                // writes into skipToIdx
                skipToIdx = predicate.parameterEndIdx;
                rememberPredicate(predicate.name, predicate.arity);
            }
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
                if (openingBraceIdx > implicationIdx) {
                    // constant
                    rememberPredicate(rule.substr(currentIdx, implicationIdx - currentIdx), 0);
                } else {
                    // unary predicate
                    rememberPredicate(rule.substr(currentIdx, openingBraceIdx - currentIdx), 1);
                }
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

bool lookUpPredicate(const std::string& predicateName, int arity)
{
    if (predicatesToAritiesMap.find(predicateName) != predicatesToAritiesMap.end()) {
        // name matched
        auto& aritiesSet = predicatesToAritiesMap[predicateName];
        return aritiesSet.find(arity) != aritiesSet.end();
    }
    return false;
}

std::string expandRuleModuleProperty(const std::string& rule)
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
                if (openingBraceIdx != std::string::npos) {
                    predicate = extractPredicate(rule, openingBraceIdx);
                } else {
                    // constant before : or :-
                    predicate = extractConstant(rule, rule.find_last_not_of(' ', result.second - 1) + 1);
                }
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
                    // constant found
                    predicate = extractConstant(rule, rule.find_last_of('.'));
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
                mpRule << queryProgramSection << "(" << predicate.name;
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
                mpRule << ", " << externalName;
                mpRule << rule.substr(endLastPredicateIdx, rule.find_last_of('.') - endLastPredicateIdx) << ".";
            }
            size_t bracketIdx = rule.find_last_of('[');
            if(done && bracketIdx != std::string::npos && bracketIdx > rule.find_last_of('.')) {
                mpRule << ", " << externalName << "." << rule.substr(rule.find_last_of('.') + 1);
            }
        } else {
            currentIdx = findNextCharNotOf(rule, " ,;.}=1234567890", rule.size(), predicate.parameterEndIdx);
            if (currentIdx != std::string::npos) {
                if (done) {
                    mpRule << ", " << externalName << "." << rule.substr(rule.find_last_of('.') + 1);
                } else {
                    mpRule << rule.substr(endLastPredicateIdx, currentIdx - endLastPredicateIdx);
                    endLastPredicateIdx = currentIdx;
                }
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

int main()
{
    // query rule
    std::string queryRule = ":~ not goalReachable. [1@2]";

    // additional rules
    std::vector<std::string> rules;

    rules.emplace_back("{goal(X,Y) : field(X,Y), not visited(X,Y)} = 1 :- not haveGold, not occurs(pickup,0), not occurs(leave,0).");
    rules.emplace_back("goalReachable :- safe(X,Y) : holds(on(X,Y),T).");
    rules.emplace_back("{occurs(A,T) : action(A)} = 1 :- timestep(T).");
    rules.emplace_back("occurs(pickup,0) :- on(X,Y), glitter(X,Y).");
    rules.emplace_back("occurs(leave,0) :- on(X,Y), initial(X,Y), haveGold.");
    rules.emplace_back("goal(X,Y) :- initial(X,Y), haveGold.");
    rules.emplace_back("#minimize {2*T@1 : maxTimestep(T)}.");
    rules.emplace_back("1{occurs(A,T) : action(A)}1 :- timestep(T).");
    rules.emplace_back("success :- #count{1,X,Y : holds(on(X,Y),T) , goal(X,Y)} = 1.");
    rules.emplace_back("holds(on(X,Y),0) :- on(X,Y).");
    rules.emplace_back("holds(heading(X),0) :- heading(X).");
    rules.emplace_back("timestep(0..X) :- maxTimestep(X).");
    rules.emplace_back("holds(on(X,Y),T+1) :- occurs(move,T), fieldAhead(X,Y,T), timestep(T). ");
    rules.emplace_back("fieldAhead(X-1,Y,T) :- field(X,Y), field(X-1,Y), holds(heading(0),T), holds(on(X,Y),T), timestep(T).");
    rules.emplace_back("fieldAhead(X+1,Y,T) :- field(X,Y), field(X+1,Y), holds(heading(2),T), holds(on(X,Y),T), timestep(T).");
    rules.emplace_back("fieldAhead(X,Y+1,T) :- field(X,Y), field(X,Y+1), holds(heading(3),T), holds(on(X,Y),T), timestep(T).");
    rules.emplace_back("fieldAhead(X,Y-1,T) :- field(X,Y), field(X,Y-1), holds(heading(1),T), holds(on(X,Y),T), timestep(T).");
    rules.emplace_back("holds(heading((X+1)\\4),T+1) :- holds(heading(X),T), occurs(turnLeft,T).");
    rules.emplace_back("holds(heading((X+3)\\4),T+1) :- holds(heading(X),T), occurs(turnRight,T).");
    rules.emplace_back("holds(on(X,Y),T+1) :- holds(on(X,Y),T), not occurs(move,T), timestep(T+1).");
    rules.emplace_back("holds(heading(X),T+1) :- holds(heading(X),T), not occurs(turnLeft,T), not occurs(turnRight,T), timestep(T+1).");
    rules.emplace_back(":- not success.");

    rules.emplace_back("1{goal(X,Y) : field(X,Y), not visited(X,Y)}1 :- not haveGold, not occurs(pickup,0), not occurs(leave,0).");
    rules.emplace_back("1{maxTimestep(0..(N*N)) : fieldSize(N)}1.");
    rules.emplace_back("{maxTimestep(id,0..(N*N)) : fieldSize(N)} = 1.");
    rules.emplace_back("timestep(0..X,id) :- maxTimestep(id,X).");
    rules.emplace_back("factA(X, Y):-factB(X).");
    rules.emplace_back("{maxTimestep(0..(N*N)) : fieldSize(N)} = 1.");
    rules.emplace_back("#minimize {T@1,id : maxTimestep(id,T)}.");
    rules.emplace_back("{goal(id,X,Y) : field(X,Y), not visited(X,Y)} = 1 :- not haveGold.");
    rules.emplace_back("fieldAhead(X-1,Y,T,id) :- field(X,Y), field(X-1,Y), holds(heading(0),T,id), holds(on(X,Y),T,id), timestep(T,id).");
    rules.emplace_back("factD(X); factE(Y,Z) :- factA(X), factZ(Y,Z).");
    rules.emplace_back(":~ not goalReachable(id) : goal(id,_,_). [1@2]");

    // facts
    std::vector<std::string> facts;
    facts.emplace_back("factA.");
    facts.emplace_back("factA(1).");
    facts.emplace_back("factB(1).");
    facts.emplace_back("factC(2).");
    facts.emplace_back("factZ(1).");
    facts.emplace_back("factZ(1251).");
    facts.emplace_back("factZ(asc, 1251).");
    facts.emplace_back("factAB(wtf, you, dick, head).");
    facts.emplace_back("factAB(factB(wtf, you, factZ(1), head)).");

    std::stringstream queryProgram;
    queryProgram << "#program " << queryProgramSection << ".\n";
    queryProgram << "#external " << externalName << ".\n";

    extractHeadPredicates(queryRule);
    for (auto& fact : facts) {
        queryProgram << expandFactModuleProperty(fact);
        extractHeadPredicates(fact);
    }

    for (auto& rule : rules) {
        extractHeadPredicates(rule);
    }

    for (auto& headPredicate : predicatesToAritiesMap) {
        for (auto& arity : headPredicate.second) {
            queryProgram << createKBCapturingRule(headPredicate.first, arity);
        }
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
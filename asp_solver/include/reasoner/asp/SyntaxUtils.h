#pragma once
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace reasoner
{
namespace asp
{
class SyntaxUtils
{
public:

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
    enum Separator
    {
        Comma,
        Colon,
        Semicolon,
        None
    };

    static std::string trim(const std::string& str)
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

    static size_t findNextChar(const std::string& predicate, const std::string& chars, size_t end, size_t start)
    {
        size_t idx = predicate.find_first_of(chars, start);
        return end <= idx ? std::string::npos : idx;
    }

    static size_t findNextCharNotOf(const std::string& predicate, const std::string& chars, size_t end, size_t start)
    {
        size_t idx = predicate.find_first_not_of(chars, start);
        return end <= idx ? std::string::npos : idx;
    }

    static bool isMinOrMax(const std::string &rule, size_t openingCurlyBracesIdx)
    {
        size_t hashIdx = rule.find_last_of('#', openingCurlyBracesIdx);
        if (hashIdx == std::string::npos) {
            return false;
        }
        std::string potentialMinMax = rule.substr(hashIdx, openingCurlyBracesIdx - hashIdx);
        potentialMinMax = trim(potentialMinMax);
        if (potentialMinMax.compare("#minimize") == 0 || potentialMinMax.compare("#maximize") == 0) {
            return true;
        } else {
            return false;
        }
    }

    static size_t findMatchingClosingBrace(const std::string& predicate, size_t openingBraceIdx)
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

    /**
     * Finds the implication of a given rule. Also handles #minimize and #maximize, as they are converted to
     * a weak constraint like ":~ not goalReachable(id) : goal(id,_,_). [1@2]"
     * @param rule
     * @return Idx of the implication ":-"
     */
    static size_t findImplication(const std::string& rule)
    {
        size_t idxs[] = {rule.find(":-"), rule.find(":~"), rule.find("#minimize"), rule.find("#maximize")};
        return *std::min_element(idxs, idxs + 4);
    }

    static Predicate extractConstant(std::string rule, size_t constantEndIdxPlusOne)
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




    static Predicate extractPredicate(std::string rule, size_t parameterStartIdx)
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

    static std::string createVariableParameters(int arity)
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

    static std::pair<Separator, size_t> determineFirstSeparator(std::pair<Separator, size_t> a, std::pair<Separator, size_t> b)
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
};
} /* namespace asp */
} /* namespace reasoner */

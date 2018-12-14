#pragma once

#include "SyntaxUtils.h"
#include <map>
#include <unordered_set>

namespace reasoner
{
namespace asp
{
class PredicateContainer
{

protected:
    std::map<std::string, std::unordered_set<int>> predicatesToAritiesMap;

    void rememberPredicate(std::string predicateName, int arity)
    {
        if (predicateName.empty()) {
            return;
        }
        predicateName = SyntaxUtils::trim(predicateName);
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

    bool lookUpPredicate(const std::string& predicateName, int arity)
    {
        if (predicatesToAritiesMap.find(predicateName) != predicatesToAritiesMap.end()) {
            // name matched
            auto& aritiesSet = predicatesToAritiesMap[predicateName];
            return aritiesSet.find(arity) != aritiesSet.end();
        }
        return false;
    }

    void extractHeadPredicates(const std::string& rule)
    {
        auto headPredicates = this->determineHeadPredicates(rule);
        for (auto hp : headPredicates) {
            rememberPredicate(hp.name, hp.arity);
        }
    }

    std::vector<SyntaxUtils::Predicate> determineHeadPredicates(const std::string& rule)
    {

        size_t implicationIdx = SyntaxUtils::findImplication(rule);
        if (implicationIdx == 0) {
            return std::vector<SyntaxUtils::Predicate>();
        }
        std::vector<SyntaxUtils::Predicate> ret;
        std::pair<SyntaxUtils::Separator, size_t> colonPair;
        std::pair<SyntaxUtils::Separator, size_t> semicolonPair;
        std::pair<SyntaxUtils::Separator, size_t> commaPair;
        std::pair<SyntaxUtils::Separator, size_t> result;
        size_t openingBraceIdx = std::string::npos;
        size_t skipToIdx = std::string::npos;
        size_t currentIdx = 0;

        bool done = false;

        while (!done) {
            colonPair =
                    std::pair<SyntaxUtils::Separator, size_t>(SyntaxUtils::Separator::Colon, SyntaxUtils::findNextChar(rule, ":", implicationIdx, currentIdx));
            semicolonPair = std::pair<SyntaxUtils::Separator, size_t>(
                    SyntaxUtils::Separator::Semicolon, SyntaxUtils::findNextChar(rule, ";", implicationIdx, currentIdx));
            commaPair =
                    std::pair<SyntaxUtils::Separator, size_t>(SyntaxUtils::Separator::Comma, SyntaxUtils::findNextChar(rule, ",", implicationIdx, currentIdx));
            result = SyntaxUtils::determineFirstSeparator(SyntaxUtils::determineFirstSeparator(colonPair, semicolonPair), commaPair);
            openingBraceIdx = SyntaxUtils::findNextChar(rule, "(", result.second, currentIdx);
            SyntaxUtils::Predicate predicate;

            // handle next predicate
            switch (result.first) {
            case SyntaxUtils::Semicolon:
            case SyntaxUtils::Comma:
                if (openingBraceIdx == std::string::npos) {
                    // constant
                    predicate.name = rule.substr(currentIdx, result.second - currentIdx);
                    predicate.arity = 0;
                    ret.push_back(predicate);
                    // rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
                } else {
                    // SyntaxUtils::Predicate predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
                    predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
                    // writes into skipToIdx
                    skipToIdx = predicate.parameterEndIdx;
                    // rememberPredicate(predicate.name, predicate.arity);
                    ret.push_back(predicate);
                }
                break;
            case SyntaxUtils::Colon:
                if (openingBraceIdx == std::string::npos) {
                    // constant
                    // rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
                    predicate.name = rule.substr(currentIdx, result.second - currentIdx);
                    predicate.arity = 0;
                    ret.push_back(predicate);
                } else {
                    // SyntaxUtils::Predicate predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
                    predicate = SyntaxUtils::extractPredicate(rule, openingBraceIdx);
                    // writes into skipToIdx
                    skipToIdx = predicate.parameterEndIdx;
                    // rememberPredicate(predicate.name, predicate.arity);
                    ret.push_back(predicate);
                }
                // skip behind conditional: ';' or '}' will end conditional
                skipToIdx = SyntaxUtils::findNextChar(rule, "}", implicationIdx, result.second + 1);
                if (skipToIdx == std::string::npos) {
                    skipToIdx = SyntaxUtils::findNextChar(rule, ";", implicationIdx, result.second + 1);
                } else {
                    skipToIdx = SyntaxUtils::findNextChar(rule, ",;", implicationIdx, skipToIdx + 1);
                    if (skipToIdx == std::string::npos) {
                        done = true;
                    }
                }
                break;
            case SyntaxUtils::None:
                if (openingBraceIdx == std::string::npos) {
                    // constant
                    // rememberPredicate(rule.substr(currentIdx, result.second - currentIdx), 0);
                    predicate.name = rule.substr(currentIdx, result.second - currentIdx);
                    predicate.arity = 0;
                    ret.push_back(predicate);
                } else {
                    if (openingBraceIdx > implicationIdx) {
                        // constant
                        // rememberPredicate(rule.substr(currentIdx, implicationIdx - currentIdx), 0);
                        predicate.name = rule.substr(currentIdx, implicationIdx - currentIdx);
                        predicate.arity = 0;
                        ret.push_back(predicate);
                    } else {
                        // unary predicate
                        predicate.name = rule.substr(currentIdx, openingBraceIdx - currentIdx);
                        predicate.arity = 1;
                        ret.push_back(predicate);
                        // rememberPredicate(rule.substr(currentIdx, openingBraceIdx - currentIdx), 1);
                    }
                }
                break;
            default:
                std::cerr << "MP: Separator Char not known!" << std::endl;
            }

            // update currentIdx
            if (skipToIdx != std::string::npos) {
                currentIdx = SyntaxUtils::findNextCharNotOf(rule, " ,;.", implicationIdx, skipToIdx + 1);
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
        return ret;
    }
};
} /* namespace asp*/
} /* namespace reasoner*/
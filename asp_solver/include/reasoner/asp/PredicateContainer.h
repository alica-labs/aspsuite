#pragma once

#include "SyntaxUtils.h"

#include <map>
#include <unordered_set>
#include <vector>

namespace reasoner
{
namespace asp
{
class PredicateContainer
{
protected:
    void rememberPredicates(const std::vector<SyntaxUtils::Predicate>& predicates);
    bool lookUpPredicate(const std::string& predicateName, int arity);
    static std::vector<SyntaxUtils::Predicate> determineHeadPredicates(const std::string& rule);

    std::map<std::string, std::unordered_set<int>> predicatesToAritiesMap;
private:
    void rememberPredicate(std::string predicateName, int arity);
};
} /* namespace asp*/
} /* namespace reasoner*/
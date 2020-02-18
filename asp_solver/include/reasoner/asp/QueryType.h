#pragma once

#include <iosfwd>

namespace reasoner
{
namespace asp
{
enum class QueryType
{
    Filter,
    Extension,
    IncrementalExtension,
    ReusableExtension,
    BackgroundKnowledge,
    Undefined
};
std::ostream& operator<<(std::ostream& os, const QueryType& queryType);
} // namespace asp
} // namespace reasoner

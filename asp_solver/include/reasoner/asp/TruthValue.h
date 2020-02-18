#pragma once

#include <iosfwd>

namespace reasoner
{
namespace asp
{
enum TruthValue
{
    False,
    True,
    Unknown
};
std::ostream& operator<<(std::ostream& os, const TruthValue& truthValue);
} // namespace asp
} // namespace reasoner

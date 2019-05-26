#pragma once

namespace reasoner
{
namespace asp
{
enum QueryType
{
    Filter,
    Extension,
    IncrementalExtension,
    ReusableExtension,
    Undefined
};

enum TruthValue
{
    False,
    True,
    Unknown
};
} /* namespace asp */
} /* namespace reasoner */

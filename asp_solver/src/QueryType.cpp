#include "reasoner/asp/QueryType.h"

#include <iostream>

namespace reasoner
{
namespace asp
{
/**
 * For getting a string representation of a type of a query.
 * @param os Outputstream
 * @param type The type of a query.
 * @return Outputstream
 */
std::ostream& operator<<(std::ostream& os, const QueryType& queryType)
{
    switch (queryType) {
    case QueryType::Extension:
        os << "Extension";
        break;
    case QueryType::Filter:
        os << "Filter";
        break;
    case QueryType::IncrementalExtension:
        os << "IncrementalExtension";
        break;
    case QueryType::ReusableExtension:
        os << "ReusableExtension";
        break;
    case QueryType ::BackgroundKnowledge:
        os << "BackgroundKnowledge";
        break;
    case QueryType::Undefined:
        os << "Undefined";
        break;
    default:
        os.setstate(std::ios_base::failbit);
    }
    return os;
}
} // namespace asp
} // namespace reasoner
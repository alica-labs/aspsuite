#include "reasoner/asp/TruthValue.h"

#include <iostream>

namespace reasoner
{
namespace asp
{
/**
 * For getting a string representation of a truth value.
 * @param os Outputstream
 * @param type The truth value.
 * @return Outputstream
 */
std::ostream& operator<<(std::ostream& os, const TruthValue& truthValue)
{
    switch (truthValue) {
    case TruthValue::True:
        os << "True";
        break;
    case TruthValue::False:
        os << "False";
        break;
    case TruthValue::Unknown:
        os << "Unkown";
        break;
    default:
        os.setstate(std::ios_base::failbit);
    }
    return os;
}
} // namespace asp
} // namespace reasoner
#include <ASPCommonsVariable.h>

namespace reasoner
{
int ASPCommonsVariable::var_id = 0;
ASPCommonsVariable::ASPCommonsVariable()
{
    ownId = var_id++;
}

ASPCommonsVariable::~ASPCommonsVariable() {}

} /* namespace reasoner */

#include "reasoner/asp/Variable.h"

namespace reasoner
{
namespace asp
{
int Variable::var_id = 0;

Variable::Variable()
{
    ownId = var_id++;
}

Variable::~Variable() {}

} /* namespace asp */
} /* namespace reasoner */

#include <asp_solver_wrapper/ASPVariable.h>

namespace alica
{
	namespace reasoner
	{
		ASPVariable::ASPVariable(uint64_t id) : SolverVariable(id)
		{
			this->id = id;
		}

		ASPVariable::~ASPVariable()
		{
		}

	} /* namespace reasoner */
} /* namespace alica */

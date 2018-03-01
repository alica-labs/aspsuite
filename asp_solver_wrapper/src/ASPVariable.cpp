#include <asp_solver_wrapper/ASPVariable.h>

namespace alica
{
	namespace reasoner
	{
		int ASPVariable::idCounter = 0;
		ASPVariable::ASPVariable()
		{
			id = idCounter++;
		}

		ASPVariable::~ASPVariable()
		{
		}

	} /* namespace reasoner */
} /* namespace alica */

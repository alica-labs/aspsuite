#pragma once

#include <alica_solver_interface/SolverVariable.h>
#include <string>
#include <memory>
#include "ASPTerm.h"

namespace alica
{
	namespace reasoner
	{

		class ASPVariable : public alica::reasoner::ASPTerm, public alica::SolverVariable
		{
		public:
			ASPVariable(uint64_t id);
			virtual ~ASPVariable();

		private:
            uint64_t id;
		};

	} /* namespace reasoner */
} /* namespace alica */

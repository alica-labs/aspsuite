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
			ASPVariable();
			virtual ~ASPVariable();

		private:
			int id;
			static int idCounter;
		};

	} /* namespace reasoner */
} /* namespace alica */

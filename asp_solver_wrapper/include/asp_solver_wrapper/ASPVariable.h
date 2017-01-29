/*
 * Variable.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLE_H_
#define INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLE_H_

#include <engine/constraintmodul/SolverVariable.h>
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
			int ownId;
			static int var_id;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_ASPVARIABLE_H_ */

/*
 * Variable.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_VARIABLE_H_
#define INCLUDE_ALICA_ASP_SOLVER_VARIABLE_H_

#include <engine/constraintmodul/SolverVariable.h>
#include <string>

namespace alica
{
	namespace reasoner
	{

		class Variable : public SolverVariable
		{
		public:
			Variable();
			virtual ~Variable();
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_VARIABLE_H_ */

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
#include <memory>

namespace alica
{
	namespace reasoner
	{

		class Variable : public std::enable_shared_from_this<Variable>, public alica::SolverVariable
		{
		public:
			Variable();
			virtual ~Variable();
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_VARIABLE_H_ */

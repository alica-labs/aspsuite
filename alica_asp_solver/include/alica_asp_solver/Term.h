/*
 * Term.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ALICA_ASP_SOLVER_TERM_H_
#define INCLUDE_ALICA_ASP_SOLVER_TERM_H_

#include <engine/constraintmodul/SolverTerm.h>
#include <string>
#include <memory>

namespace alica
{
	namespace reasoner
	{

		class Term : public std::enable_shared_from_this<Term>, public alica::SolverTerm
		{
		public:
			Term();
			virtual ~Term();
			std::string rule;
			std::string backgroundFile;
		};

	} /* namespace reasoner */
} /* namespace alica */

#endif /* INCLUDE_ALICA_ASP_SOLVER_TERM_H_ */

/*
 * Variable.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#ifndef INCLUDE_ASP_SOLVER_ASPVARIABLE_H_
#define INCLUDE_ASP_SOLVER_ASPVARIABLE_H_

#include <string>
#include <memory>

#include "../include/asp_commons/ASPTerm.h"

namespace reasoner
{

	class ASPVariable : public reasoner::ASPTerm
	{
	public:
		ASPVariable();
		virtual ~ASPVariable();

	private:
		int ownId;
		static int var_id;
	};

} /* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPVARIABLE_H_ */

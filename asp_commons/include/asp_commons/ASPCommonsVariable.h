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

#include "asp_commons/ASPCommonsTerm.h"

namespace reasoner
{

	class ASPCommonsVariable : public reasoner::ASPCommonsTerm
	{
	public:
		ASPCommonsVariable();
		virtual ~ASPCommonsVariable();

	private:
		int ownId;
		static int var_id;
	};

} /* namespace reasoner */

#endif /* INCLUDE_ASP_SOLVER_ASPVARIABLE_H_ */

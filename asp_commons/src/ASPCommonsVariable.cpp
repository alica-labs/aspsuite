/*
 * Variable.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <ASPCommonsVariable.h>

namespace reasoner
{
	int ASPCommonsVariable::var_id = 0;
	ASPCommonsVariable::ASPCommonsVariable()
	{
		ownId = var_id++;

	}

	ASPCommonsVariable::~ASPCommonsVariable()
	{
	}

} /* namespace reasoner */

/*
 * Variable.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/Variable.h>

namespace alica
{
	namespace reasoner
	{
		int Variable::var_id = 0;
		Variable::Variable()
		{
			ownId = var_id++;

		}

		Variable::~Variable()
		{
			// TODO Auto-generated destructor stub
		}

	} /* namespace reasoner */
} /* namespace alica */

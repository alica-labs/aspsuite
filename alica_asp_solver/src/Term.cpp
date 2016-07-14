/*
 * Term.cpp
 *
 *  Created on: Jul 13, 2016
 *      Author: Stefan Jakob
 */

#include <alica_asp_solver/Term.h>

namespace alica
{
	namespace reasoner
	{

		Term::Term()
		{
			this->backgroundFile = "";
			this->rule = "";
		}

		Term::~Term()
		{
		}

	} /* namespace reasoner */
} /* namespace alica */

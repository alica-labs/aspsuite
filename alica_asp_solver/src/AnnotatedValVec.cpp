/*
 * AnnotatedValVec.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#include "alica_asp_solver/AnnotatedValVec.h"

namespace alica
{
	namespace reasoner
	{

//		AnnotatedValVec::AnnotatedValVec()
//		{
//			this->id = -1;
//			this->query = nullptr;
//		}

		AnnotatedValVec::AnnotatedValVec(long id, vector<Gringo::ValVec> values, shared_ptr<ASPQuery> query)
		{
			this->id = id;
			this->values = values;
			this->query = query;
		}

		AnnotatedValVec::~AnnotatedValVec()
		{
		}

	} /* namespace reasoner */
} /* namespace alica */

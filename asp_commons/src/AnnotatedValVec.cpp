/*
 * AnnotatedValVec.cpp
 *
 *  Created on: Nov 5, 2016
 *      Author: stefan
 */

#include "../include/asp_commons/AnnotatedValVec.h"
#include "../include/asp_commons/ASPQuery.h"

namespace reasoner
{

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

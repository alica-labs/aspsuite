/*
 * ConceptNetConcept.cpp
 *
 *  Created on: Mar 23, 2017
 *      Author: stefan
 */

#include "containers/ConceptNetConcept.h"

namespace cng
{

	ConceptNetConcept::ConceptNetConcept(QString term,	QString senseLabel,	QString cn5ID)
	{
		this->term = term;
		this->senseLabel = senseLabel;
		this->cn5ID = cn5ID;
	}

	ConceptNetConcept::~ConceptNetConcept()
	{
	}

} /* namespace cng */

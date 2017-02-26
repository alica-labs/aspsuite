/*
 * ConceptNetCall.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include "containers/ConceptNetCall.h"
#include "containers/ConceptNetEdge.h"

#include <string>
#include <sstream>

namespace cng
{

	ConceptNetCall::ConceptNetCall(QString id)
	{
		this->id = id;
		this->nextEdgesPage = nextEdgesPage;
	}

	ConceptNetCall::~ConceptNetCall()
	{
	}

	std::string ConceptNetCall::toString()
	{
		std::stringstream ss;
		ss << "ConceptNetCall:" << std::endl;
		ss << "ID: " << this->id.toStdString() << std::endl;
		ss << "Edges:" << std::endl;
		for(auto edge : this->edges)
		{
			ss << edge->toString();
		}
		ss << "NextEdgePage: " << this->nextEdgesPage.toStdString() << std::endl;
		return ss.str();
	}

} /* namespace cng */

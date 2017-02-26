/*
 * ConceptNetEdge.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include "containers/ConceptNetEdge.h"

#include <sstream>
#include <string>

namespace cng
{

	ConceptNetEdge::ConceptNetEdge(QString id, QString language, QString firstConcept, QString secondConcept,
									QString relation, double weight)
	{
		this->id = id;
		this->language = language;
		this->firstConcept = firstConcept;
		this->secondConcept = secondConcept;
		this->weight = weight;
		this->relation = relation;
	}

	ConceptNetEdge::~ConceptNetEdge()
	{
	}

	std::string ConceptNetEdge::toString()
	{
		std::stringstream ss;
		ss << "Edge: " << firstConcept.toStdString() << " " << relation.toStdString() << " " << secondConcept.toStdString()
				<< " Weight: " << weight << " Number of sources: " << this->sources.size() << std::endl;
		return ss.str();
	}

} /* namespace cng */

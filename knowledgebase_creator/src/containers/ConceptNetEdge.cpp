/*
 * ConceptNetEdge.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#include "containers/ConceptNetEdge.h"
#include "containers/ConceptNetConcept.h"

#include <sstream>
#include <string>

namespace kbcr
{

ConceptNetEdge::ConceptNetEdge(QString id, QString language, std::shared_ptr<ConceptNetConcept> firstConcept, std::shared_ptr<ConceptNetConcept> secondConcept,
        QString relation, double weight)
{
    this->id = id;
    this->language = language;
    this->firstConcept = firstConcept;
    this->secondConcept = secondConcept;
    this->weight = weight;
    this->relation = relation;
}

ConceptNetEdge::~ConceptNetEdge() {}

std::string ConceptNetEdge::toString()
{
    std::stringstream ss;
    ss << "Edge: " << this->firstConcept->term.toStdString() << " Sense: " << this->firstConcept->senseLabel.toStdString() << " "
       << this->relation.toStdString() << " " << this->secondConcept->term.toStdString() << " Sense: " << this->secondConcept->senseLabel.toStdString()
       << " Weight: " << this->weight << " Number of sources: " << this->sources.size() << std::endl;
    return ss.str();
}

} /* namespace kbcr */

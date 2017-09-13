/*
 * ConceptNetEdge.h
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#ifndef SRC_CONTAINERS_CONCEPTNETEDGE_H_
#define SRC_CONTAINERS_CONCEPTNETEDGE_H_

#include <string>
#include <vector>
#include <memory>

#include <QString>

namespace kbcr
{

	/**
	 * Class representing an edge of the concept net graph
	 */
	class ConceptNetConcept;
	class ConceptNetEdge
	{
	public:
		ConceptNetEdge(QString id, QString language, std::shared_ptr<ConceptNetConcept> firstConcept, std::shared_ptr<ConceptNetConcept> secondConcept, QString relation ,double weight);
		virtual ~ConceptNetEdge();

		QString id;
		QString language;
		std::shared_ptr<ConceptNetConcept> firstConcept;
		std::shared_ptr<ConceptNetConcept> secondConcept;
		QString relation;
		double weight;
		std::vector<QString> sources;

		std::string toString();

	};

} /* namespace kbcr */

#endif /* SRC_CONTAINERS_CONCEPTNETEDGE_H_ */

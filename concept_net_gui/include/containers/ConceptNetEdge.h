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
#include <QString>

namespace cng
{

	/**
	 * Class representing an edge of the concept net graph
	 */
	class ConceptNetEdge
	{
	public:
		ConceptNetEdge(QString id, QString language, QString firstConcept, QString secondConcept, QString relation ,double weight);
		virtual ~ConceptNetEdge();

		QString id;
		QString language;
		QString firstConcept;
		QString secondConcept;
		QString relation;
		double weight;
		std::vector<QString> sources;
		std::string toString();

	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETEDGE_H_ */

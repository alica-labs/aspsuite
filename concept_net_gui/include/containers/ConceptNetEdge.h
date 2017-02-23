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

namespace cng
{

	class ConceptNetEdge
	{
	public:
		ConceptNetEdge(std::string id, std::string language, std::string firstConcept, std::string secondConcept, std::string relation ,double weight);
		virtual ~ConceptNetEdge();

		std::string id;
		std::string language;
		std::string firstConcept;
		std::string secondConcept;
		std::string relation;
		double weight;
		std::vector<std::string> sources;
		std::string toString();

	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETEDGE_H_ */

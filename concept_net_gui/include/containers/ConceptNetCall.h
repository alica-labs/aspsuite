/*
 * ConceptNetCall.h
 *
 *  Created on: Feb 23, 2017
 *      Author: stefan
 */

#ifndef SRC_CONTAINERS_CONCEPTNETCALL_H_
#define SRC_CONTAINERS_CONCEPTNETCALL_H_

#include <string>
#include <vector>
#include <memory>

namespace cng
{

	class ConceptNetEdge;
	class ConceptNetCall
	{
	public:
		ConceptNetCall(std::string id);
		virtual ~ConceptNetCall();
		std::string id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		std::string nextEdgesPage;
		std::string toString();
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

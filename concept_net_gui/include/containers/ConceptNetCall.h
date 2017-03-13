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
#include <QString>

namespace cng
{

	class ConceptNetEdge;
	/**
	 * Class holding the complete information of a concept net query call
	 */
	class ConceptNetCall
	{
	public:
		ConceptNetCall(QString id);
		virtual ~ConceptNetCall();

		QString id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		QString nextEdgesPage;

		std::string toString();
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

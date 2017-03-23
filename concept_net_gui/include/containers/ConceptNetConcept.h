/*
 * ConceptNetConcept.h
 *
 *  Created on: Mar 23, 2017
 *      Author: stefan
 */

#ifndef SRC_CONTAINERS_CONCEPTNETCONCEPT_H_
#define SRC_CONTAINERS_CONCEPTNETCONCEPT_H_

#include <QString>
#include <QObject>

namespace cng
{

	class ConceptNetConcept : public QObject
	{
	Q_OBJECT
	public:
		ConceptNetConcept(QString term,	QString senseLabel,	QString cn5ID);
		virtual ~ConceptNetConcept();

		QString term;
		QString senseLabel;
		QString cn5ID;
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCONCEPT_H_ */

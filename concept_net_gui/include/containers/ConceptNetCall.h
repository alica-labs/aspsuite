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

#include <QJsonObject>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace cng
{

	class ConceptNetEdge;
	/**
	 * Class holding the complete information of a concept net query call
	 */
	class ConceptNetCall : public QObject
	{
	Q_OBJECT
	public:
		ConceptNetCall(QString id);
		virtual ~ConceptNetCall();


		QNetworkAccessManager *nam;

		QString id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		std::vector<std::string> adjectives;
		std::vector<std::string> concepts;
		QString nextEdgesPage;

		std::string toString();

	public slots:
		void adjectiveChecked(QNetworkReply* reply);
		void findAdjectives();

	private:
		int currentAdjectiveIndex;

	signals:
		void nextAdjective();
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

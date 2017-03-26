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
#include <map>
#include <memory>

#include <QJsonObject>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace cng
{
	class ConceptNetGui;
	class ConceptNetEdge;
	/**
	 * Class holding the complete information of a concept net query call
	 */
	class ConceptNetCall : public QObject
	{
	Q_OBJECT
	public:
		ConceptNetCall(ConceptNetGui* gui, QString id, QString queryConcept);
		virtual ~ConceptNetCall();


		QNetworkAccessManager *nam;
//		QNetworkAccessManager *nam2;
		QNetworkAccessManager *checkNAM;

		QString id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		std::map<QString, double> adjectives;
		std::vector<QString> concepts;
		std::map<QString, double> conceptsToCheck;
		std::map<QString, double> checkedConcepts;
		std::map<QString, std::vector<QString>> adjectiveAntonymMap;
		QString nextEdgesPage;
		QString queryConcept;
		std::pair<QString, QString> currentAntonymCheck;

		std::string toString();

	public slots:
//		void adjectiveChecked(QNetworkReply* reply);
		void checkFirstAdjectives();
		void inconsistencySearch();
		void findInconsistencies();
		void removeIfAntonym(QNetworkReply* reply);
//		void mapAntonyms(QNetworkReply* reply);
		void collectConcepts(QNetworkReply* reply);

	private:
		int currentAdjectiveIndex;
//		void collectAntonyms();
		ConceptNetGui* gui;
		QString trimTerm(QString term);
		bool conceptContainsUTF8(QString concept);

	signals:
		void closeLoopFirstAdjectives();
		void closeLoopAdjectiveGathering();
		void closeLoop2();
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

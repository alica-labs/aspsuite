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
#include <QThread>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace cng
{
	class ConceptNetGui;
	class ConceptNetEdge;
	/**
	 * Class holding the complete information of a concept net query call
	 */
	class ConceptNetCall : public QThread
	{
	Q_OBJECT
	public:
		ConceptNetCall(ConceptNetGui* gui, QString id, QString queryConcept);
		virtual ~ConceptNetCall();


		QNetworkAccessManager *nam; // namNumber = 0
		QNetworkAccessManager *checkNAM; // namNumber = 1

		QString id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		std::map<QString, std::shared_ptr<ConceptNetEdge>> adjectives;
		std::vector<QString> concepts;
		std::map<QString, std::shared_ptr<ConceptNetEdge>> conceptsToCheck;
		std::map<QString, std::shared_ptr<ConceptNetEdge>> checkedConcepts;
		std::map<QString, std::vector<QString>> adjectiveAntonymMap;
		QString nextEdgesPage;
		QString queryConcept;
		std::pair<std::pair<QString, std::shared_ptr<ConceptNetEdge>>,
				std::pair<QString, std::shared_ptr<ConceptNetEdge>>> currentAntonymCheck;

		std::string toString();
		void findInconsistencies();

	public slots:
		void removeIfAntonym(QNetworkReply* reply);
		void collectConcepts(QNetworkReply* reply);

	private:
		void gatherConcepts(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck);
		void checkAdjectives(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck);
		ConceptNetGui* gui;
		QString trimTerm(QString term);
		std::map<QString, std::shared_ptr<ConceptNetEdge>>::iterator it;
		bool conceptDeleted;
		/**
		 * Executes http get request
		 */
		void callUrl(QUrl url, QNetworkAccessManager* n);
		bool conceptContainsUTF8(QString concept);
		std::vector<std::vector<std::shared_ptr<ConceptNetEdge>>> inconsistencies;
		std::shared_ptr<ConceptNetEdge> extractCNEdge(QJsonObject edge);
		bool newConceptFound;

	signals:
		void closeLoopAntonym();
		void closeLoopAdjectiveGathering();
	};

} /* namespace cng */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

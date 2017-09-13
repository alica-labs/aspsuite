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

namespace kbcr
{
	class KnowledgebaseCreator;
	class ConceptNetEdge;
	/**
	 * Class holding the complete information of a concept net query call
	 */
	class ConceptNetCall : public QThread
	{
	Q_OBJECT
	public:
		ConceptNetCall(KnowledgebaseCreator* gui, QString id, QString queryConcept);
		virtual ~ConceptNetCall();


		QNetworkAccessManager *checkNAM;
		QNetworkAccessManager *nam2;

		QString id;
		std::vector<std::shared_ptr<ConceptNetEdge>> edges;
		std::map<QString, std::shared_ptr<ConceptNetEdge>> adjectives;
		std::vector<QString> concepts;
		std::map<QString, std::shared_ptr<ConceptNetEdge>> conceptsToCheck;
		std::map<QString, std::pair<QString, std::shared_ptr<ConceptNetEdge>>> gatheredConcepts;
		std::map<QString, std::vector<QString>> adjectiveAntonymMap;
		std::map<QString, std::shared_ptr<std::vector<QString>>> gatherMap;
		QString nextEdgesPage;
		QString queryConcept;
		QString currentConcept;
		std::pair<std::pair<QString, std::shared_ptr<ConceptNetEdge>>,
				std::pair<QString, std::shared_ptr<ConceptNetEdge>>> currentAntonymCheck;

		std::string toString();
		void findInconsistencies();

	public slots:
		void collectConcepts(QNetworkReply* reply);
		void mapAntonyms(QNetworkReply* reply);

	private:
		void gatherConcepts(std::map<QString, std::shared_ptr<ConceptNetEdge>> toCheck);
		void checkAdjectives();
		void checkGatheredConcepts();
		KnowledgebaseCreator* gui;
		QString trimTerm(QString term);
		/**
		 * Executes http get request
		 */
		void collectAntonyms();
		void callUrl(QUrl url, QNetworkAccessManager* n);
		bool conceptContainsUTF8(QString concept);
		std::vector<std::vector<std::shared_ptr<ConceptNetEdge>>> inconsistencies;
		std::shared_ptr<ConceptNetEdge> extractCNEdge(QJsonObject edge);
		bool newConceptFound;
		QString currentAdjective;

	signals:
		void closeLoopAntonym();
		void closeLoopAdjectiveGathering();
		void closeLoop2();
	};

} /* namespace kbcr */

#endif /* SRC_CONTAINERS_CONCEPTNETCALL_H_ */

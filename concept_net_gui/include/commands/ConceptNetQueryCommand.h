/*
 * ConceptNetQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_
#define SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_

#include <commands/Command.h>

#include <memory>
#include <string>

#include <QJsonObject>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

//#define ConceptNetQueryCommandDebug

namespace cng
{

	class ConceptNetCall;
	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to create a ConceptNet Query
	 */
	class ConceptNetQueryCommand : public Command, public std::enable_shared_from_this<ConceptNetQueryCommand>
	{
	Q_OBJECT
	public:
		ConceptNetQueryCommand(ConceptNetGui* gui, QString query);
		virtual ~ConceptNetQueryCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString query;
		/**
		 * Encapsulates the results given from COncept Net
		 */
		ConceptNetCall* currentConceptNetCall;

	private slots:
		/**
		 * Slot accepting the http request from concept net
		 */
		void conceptNetCallFinished(QNetworkReply* reply);
		/**
		 * Slot used to parse the result given JSON to asp conform syntax
		 */
		void extractASPPredicates();

	private:
		/**
		 * Handles the http requests
		 */
		QNetworkAccessManager *nam;
		/**
		 * Extracts ASP predicates from concept net answer and adds weight and number of sources
		 */
		QString createWeightedASPPredicates();
		/**
		 * Extracts ASP predicates from concept net answer and adds weight divided by number of sources
		 */
		QString createAvgWeightedASPPredicates();
		/**
		 * Extracts ASP predicates from concept net answer
		 */
		QString createASPPredicates();
		/**
		 * removes dots, commas and spaces from concept net answer
		 */
		QString conceptToASPPredicate(QString concept);
		/**
		 * Checks if query is one of the concept net base relations
		 */
		bool isConceptNetRealtion(QString query);
		/**
		 * Informs the user that the given query is not supported by concept net
		 */
		void handleWrongInput();
		/**
		 * Handles the 3 query types:
		 * 1: plain concept
		 * 2: relation(concept, wildcard)
		 * 3: relation(wildcard, concept)
		 */
		void handleQuery();
		/**
		 * Removes leading /c/en/ from a concept net term representing a concept
		 */
		QString trimTerm(QString term);
		/**
		 * Executes http get request
		 */
		void callUrl(QUrl url);

	signals:
		/**
		 * Signal used as soon as the extraction from concept net is finished
		 */
		void jsonExtracted();

	};

} /* namespace cng */

#endif /* SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_ */

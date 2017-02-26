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
		ConceptNetCall* currentConceptNetCall;

	private slots:
		void conceptNetCallFinished(QNetworkReply* reply);
		void extractASPPredicates();

	private:
		QNetworkAccessManager *nam;
		QString createWeightedASPPredicates();
		QString createAvgWeightedASPPredicates();
		QString createASPPredicates();
		QString conceptToASPPredicate(QString concept);
		bool isConceptNetRealtion(QString query);
		void handleWrongInput();
		void handleQuery();
		QString trimTerm(QString term);
		void callUrl(QUrl url);

	signals:
		void jsonExtracted();

	};

} /* namespace cng */

#endif /* SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_ */

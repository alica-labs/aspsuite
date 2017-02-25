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
	class ConceptNetQueryCommand : public Command, public enable_shared_from_this<ConceptNetQueryCommand>
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
		shared_ptr<ConceptNetCall> currentConceptNetCall;

	private slots:
		void conceptNetCallFinished(QNetworkReply* reply);
		void extractASPPredicates();

	private:
		QNetworkAccessManager *nam;
		std::string conceptToASPPredicate(std::string concept);
		QString createWeightedASPPredicates();
		QString createAvgWeightedASPPredicates();
		QString createASPPredicates();
		bool isConceptNetRealtion(QString query);
		void handleWrongInput();
		void handleQuery();

	signals:
		void jsonExtracted();

	};

} /* namespace cng */

#endif /* SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_ */

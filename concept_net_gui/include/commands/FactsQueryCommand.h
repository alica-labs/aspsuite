/*
 * FactsQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_FACTSQUERYCOMMAND_H_
#define SRC_COMMANDS_FACTSQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <QJsonObject>

namespace cng
{
	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to create a FactsQuery
	 */
	class FactsQueryCommand : public Command, public enable_shared_from_this<FactsQueryCommand>
	{
		Q_OBJECT
	public:
		FactsQueryCommand(ConceptNetGui* gui, QString factsString);
		virtual ~FactsQueryCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString factsString;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_FACTSQUERYCOMMAND_H_ */

/*
 * VariableQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_
#define INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>
#include <string>

#include <QJsonObject>

namespace kbcr
{

	class KnowledgebaseCreator;
	/**
	 * Class inheriting from Command interface used to create a VariableQuery
	 */
	class VariableQueryCommand : public Command, public std::enable_shared_from_this<VariableQueryCommand>
	{
		Q_OBJECT
	public:
		VariableQueryCommand(KnowledgebaseCreator* gui, QString program);
		virtual ~VariableQueryCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		KnowledgebaseCreator* gui;
		QString program;
		QString toShow;
	};

} /* namespace kbcr */

#endif /* INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_ */

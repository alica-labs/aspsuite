/*
 * LoadLogicProgramCommand.h
 *
 *  Created on: Jan 21, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_
#define INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_

#include <QString>
#include <QJsonObject>
#include <QByteArray>

#include <commands/Command.h>
#include <memory>

namespace kbcr
{
	class KnowledgebaseCreator;
	/**
	 * Class inheriting from Command interface used to load a BackgroundKnowledge file
	 */
	class LoadBackgroundKnowledgeCommand : public Command, public std::enable_shared_from_this<LoadBackgroundKnowledgeCommand>
	{
		Q_OBJECT
	public:
		LoadBackgroundKnowledgeCommand(KnowledgebaseCreator* gui, QString fileName);
		virtual ~LoadBackgroundKnowledgeCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		KnowledgebaseCreator* gui;
		QByteArray fileContent;
		QString fileName;
	};

} /* namespace kbcr */

#endif /* INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_ */

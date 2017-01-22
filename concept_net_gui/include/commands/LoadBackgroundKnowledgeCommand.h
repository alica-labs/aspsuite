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

namespace cng
{
	class ConceptNetGui;
	class LoadBackgroundKnowledgeCommand : public Command, public enable_shared_from_this<LoadBackgroundKnowledgeCommand>
	{
	public:
		LoadBackgroundKnowledgeCommand(ConceptNetGui* gui, QString fileName);
		virtual ~LoadBackgroundKnowledgeCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QByteArray fileContent;
		QString fileName;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_ */

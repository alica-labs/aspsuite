/*
 * LoadLogicProgramCommand.h
 *
 *  Created on: Jan 21, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_
#define INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_

#include <QByteArray>
#include <QString>

#include <commands/Command.h>
#include <memory>

namespace cng
{
	class ConceptNetGui;
	class LoadBackgroundKnowledgeCommand : public Command, public enable_shared_from_this<LoadBackgroundKnowledgeCommand>
	{
	public:
		LoadBackgroundKnowledgeCommand(ConceptNetGui* gui, QByteArray file, QString fileName);
		virtual ~LoadBackgroundKnowledgeCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
		QByteArray file;
		QString fileName;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_LOADBACKGROUNDKNOWLEDGECOMMAND_H_ */

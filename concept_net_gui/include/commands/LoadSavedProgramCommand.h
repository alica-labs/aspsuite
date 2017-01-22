/*
 * LoadSavedProgramCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_
#define SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_

#include <commands/Command.h>
#include <memory>

#include <QJsonObject>
#include <QByteArray>
#include <QJsonDocument>

namespace cng
{
	class ConceptNetGui;
	class LoadSavedProgramCommand : public Command, public enable_shared_from_this<LoadSavedProgramCommand>
	{
	public:
		LoadSavedProgramCommand(ConceptNetGui* gui, QByteArray loadedData);
		virtual ~LoadSavedProgramCommand();

		void execute();
		void undo();

		QJsonObject toJSON();
		QByteArray loadedData;

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_ */

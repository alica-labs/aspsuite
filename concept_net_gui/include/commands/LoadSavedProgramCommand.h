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
//#include <chrono>

namespace cng
{
	class SettingsDialog;
	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to load a saved Json program
	 */
	class LoadSavedProgramCommand : public Command, public std::enable_shared_from_this<LoadSavedProgramCommand>
	{
	Q_OBJECT
	public:
		LoadSavedProgramCommand(ConceptNetGui* gui, QString fileName, QByteArray loadedData);
		virtual ~LoadSavedProgramCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		QByteArray loadedData;
		QString fileName;
		ConceptNetGui* gui;

//		std::chrono::_V2::system_clock::time_point start;

	signals:
		void cn5CallFinished();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_ */

/*
 * GroundCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_GROUNDCOMMAND_H_
#define SRC_COMMANDS_GROUNDCOMMAND_H_

#include <QString>
#include <QJsonObject>

#include <commands/Command.h>
#include <memory>

namespace cng
{
	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to call the solvers Ground method
	 */
	class GroundCommand : public Command, public std::enable_shared_from_this<GroundCommand>
	{
		Q_OBJECT
	public:
		GroundCommand(ConceptNetGui* gui, QString program);
		virtual ~GroundCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		ConceptNetGui* gui;
		QString program;
		QString programSection;
		std::string historyProgramSection;

	private:
		void extractProgramSection();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_GROUNDCOMMAND_H_ */

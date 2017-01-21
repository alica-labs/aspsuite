/*
 * GroundCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_GROUNDCOMMAND_H_
#define SRC_COMMANDS_GROUNDCOMMAND_H_

#include <QString>

#include <commands/Command.h>
#include <memory>

namespace cng
{
	class ConceptNetGui;
	class GroundCommand : public Command, public enable_shared_from_this<GroundCommand>
	{
	public:
		GroundCommand(ConceptNetGui* gui, QString program);
		virtual ~GroundCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
		QString program;
		string programSection;

	private:
		string extractProgramSection();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_GROUNDCOMMAND_H_ */

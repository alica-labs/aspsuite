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

namespace cng
{
	class ConceptNetGui;
	class LoadSavedProgramCommand : public Command, public enable_shared_from_this<LoadSavedProgramCommand>
	{
	public:
		LoadSavedProgramCommand(ConceptNetGui* gui);
		virtual ~LoadSavedProgramCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_ */

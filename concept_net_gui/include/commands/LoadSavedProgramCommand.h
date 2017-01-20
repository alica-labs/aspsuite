/*
 * LoadSavedProgramCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_
#define SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class LoadSavedProgramCommand : public Command
	{
	public:
		LoadSavedProgramCommand();
		virtual ~LoadSavedProgramCommand();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_LOADSAVEDPROGRAMCOMMAND_H_ */

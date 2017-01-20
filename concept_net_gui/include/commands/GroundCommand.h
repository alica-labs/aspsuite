/*
 * GroundCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_GROUNDCOMMAND_H_
#define SRC_COMMANDS_GROUNDCOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class GroundCommand : public Command
	{
	public:
		GroundCommand();
		virtual ~GroundCommand();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_GROUNDCOMMAND_H_ */

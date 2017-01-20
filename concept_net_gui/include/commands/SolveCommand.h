/*
 * SolveCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_SOLVECOMMAND_H_
#define SRC_COMMANDS_SOLVECOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class SolveCommand : public Command
	{
	public:
		SolveCommand();
		virtual ~SolveCommand();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_SOLVECOMMAND_H_ */

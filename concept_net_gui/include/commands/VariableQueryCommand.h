/*
 * VariableQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_
#define INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class VariableQueryCommand : public Command
	{
	public:
		VariableQueryCommand();
		virtual ~VariableQueryCommand();
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_ */

/*
 * FactsQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_FACTSQUERYCOMMAND_H_
#define SRC_COMMANDS_FACTSQUERYCOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class FactsQueryCommand : public Command
	{
	public:
		FactsQueryCommand();
		virtual ~FactsQueryCommand();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_FACTSQUERYCOMMAND_H_ */

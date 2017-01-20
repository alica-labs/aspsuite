/*
 * ConceptNetQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_
#define SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_

#include <commands/Command.h>

namespace cng
{

	class ConceptNetQueryCommand : public Command
	{
	public:
		ConceptNetQueryCommand();
		virtual ~ConceptNetQueryCommand();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_CONCEPTNETQUERYCOMMAND_H_ */

/*
 * FactsQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_FACTSQUERYCOMMAND_H_
#define SRC_COMMANDS_FACTSQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>

namespace cng
{
	class ConceptNetGui;
	class FactsQueryCommand : public Command, public enable_shared_from_this<FactsQueryCommand>
	{
	public:
		FactsQueryCommand(ConceptNetGui* gui);
		virtual ~FactsQueryCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_FACTSQUERYCOMMAND_H_ */

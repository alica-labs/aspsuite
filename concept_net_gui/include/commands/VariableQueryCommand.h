/*
 * VariableQueryCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_
#define INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_

#include <commands/Command.h>
#include <memory>

namespace cng
{

	class ConceptNetGui;
	class VariableQueryCommand : public Command, public enable_shared_from_this<VariableQueryCommand>
	{
	public:
		VariableQueryCommand(ConceptNetGui* gui);
		virtual ~VariableQueryCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_VARIABLEQUERYCOMMAND_H_ */

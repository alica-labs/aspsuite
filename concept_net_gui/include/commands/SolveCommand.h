/*
 * SolveCommand.h
 *
 *  Created on: Jan 20, 2017
 *      Author: stefan
 */

#ifndef SRC_COMMANDS_SOLVECOMMAND_H_
#define SRC_COMMANDS_SOLVECOMMAND_H_

#include <commands/Command.h>
#include <memory>

namespace cng
{

	class ConceptNetGui;
	class SolveCommand : public Command, public enable_shared_from_this<SolveCommand>
	{
	public:
		SolveCommand(ConceptNetGui* gui);
		virtual ~SolveCommand();

		void execute();
		void undo();

		ConceptNetGui* gui;
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_SOLVECOMMAND_H_ */

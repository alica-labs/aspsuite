/*
 * NewSolverCommand.h
 *
 *  Created on: Jan 19, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_
#define INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_

#include "../include/commands/Command.h"
#include <vector>
#include <memory>

#include <QJsonObject>

namespace cng
{

	class SolverSettings;
	class ConceptNetGui;
	class NewSolverCommand : public Command, public enable_shared_from_this<NewSolverCommand>
	{
	public:
		NewSolverCommand(ConceptNetGui* gui, shared_ptr<SolverSettings> settings);
		virtual ~NewSolverCommand();

		void execute();
		void undo();

		QJsonObject toJSON();

		shared_ptr<SolverSettings> settings;
		ConceptNetGui* gui;

	private:
		void getDefaultArguments();
	};

} /* namespace cng */

#endif /* INCLUDE_COMMANDS_NEWSOLVERCOMMAND_H_ */

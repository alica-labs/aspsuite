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
#include <clingo/clingocontrol.hh>

#include <QJsonObject>

namespace cng
{

	class ConceptNetGui;
	/**
	 * Class inheriting from Command interface used to call the solvers solve method
	 */
	class SolveCommand : public Command, public enable_shared_from_this<SolveCommand>
	{
		Q_OBJECT
	public:
		SolveCommand(ConceptNetGui* gui);
		virtual ~SolveCommand();

		void execute();
		void undo();

		QJsonObject toJSON();
		vector<Gringo::ValVec> getCurrentModels();
		bool isSatisfiable();

		ConceptNetGui* gui;

	private:
		bool satisfiable;
		vector<Gringo::ValVec> currentModels;

		void printModels();
		void printSortedModels();
	};

} /* namespace cng */

#endif /* SRC_COMMANDS_SOLVECOMMAND_H_ */

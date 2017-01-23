/*
 * CommandHistoryHandler.h
 *
 *  Created on: Jan 23, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_
#define INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_

#include <QObject>
#include <memory>

using namespace std;

namespace cng
{

	class Command;
	class ConceptNetGui;
	/**
	 * Handles everything related to the command history
	 */
	class CommandHistoryHandler : public QObject
	{
	Q_OBJECT
	public:
		/**
		 * Constructor
		 * @param gui pointer to the main window handler
		 */
		CommandHistoryHandler(ConceptNetGui * gui);
		virtual ~CommandHistoryHandler();
		/**
		 * Add command to history
		 * @param cmd command to add
		 */
		void addToCommandHistory(shared_ptr<Command> cmd);
		/**
		 * Remove command from history
		 * @param cmd command to be removed
		 */
		void removeFromCommandHistory(shared_ptr<Command> cmd);

	public slots:
		/**
		 * Slot to fill command history table view
		 */
		void fillCommandHistory();
		/**
		 * Slot to redraw the command history
		 */
		void drawHistoryTable();
		/**
		 * Slot used by ctrl+z to call undo on the last element of the command history
		 */
		void undoHistory();
		/**
		 * Slot used by esc button to remove focus from the text area
		 */
		void removeFocus();

	private:
		/**
		 * Pointer to main Gui handler
		 */
		ConceptNetGui * gui;

		/**
		 * Add New Solver element to history
		 */
		void addNewSolverCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Change Solver Settings element to history
		 */
		void addChangeSolverSettingsCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Concept Net Query element to history
		 */
		void addConceptNetQueryCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Ground element to history
		 */
		void addGroundCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Load Saved Program element to history
		 */
		void addLoadSavedProgramCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Solve element to history
		 */
		void addSolveCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Variable Query element to history
		 */
		void addVariableQueryCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Load Background Knowledge element to history
		 */
		void addLoadBackgroundKnowledgeCommandToHistory(shared_ptr<Command> cmd, int pos);
		/**
		 * Add Facts Query element to history
		 */
		void addFactsQueryCommandToHistory(shared_ptr<Command> cmd, int pos);
	};

} /* namespace cng */

#endif /* INCLUDE_HANDLER_COMMANDHISTORYHANDLER_H_ */

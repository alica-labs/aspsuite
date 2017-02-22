#ifndef CONCEPTNETGUI_H
#define CONCEPTNETGUI_H

#include <QMainWindow>
#include <QtGui>
#include <QMessageBox>
#include <QShortcut>

#include <iostream>
#include <memory>

#include <clingo/clingocontrol.hh>

//#define GUIDEUG

using namespace std;

namespace reasoner
{
	class ASPSolver;
}

namespace Ui
{
	class ConceptNetGui;
}

namespace cng
{
	class SolverSettings;
	class SettingsDialog;
	class NewSolverDialog;
	class Command;
	class NewSolverCommand;
	class ChangeSolverSettingsCommand;
	class ConceptNetQueryCommand;
	class FactsQueryCommand;
	class GroundCommand;
	class LoadSavedProgramCommand;
	class SolveCommand;
	class VariableQueryCommand;
	class LoadBackgroundKnowledgeCommand;
	class SaveLoadHandler;
	class CommandHistoryHandler;
	/**
	 * Class containing the main window and managing the connects, handler and provides the command history
	 */
	class ConceptNetGui : public QMainWindow
	{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 * @param parent pointer to main widget
		 */
		explicit ConceptNetGui(QWidget *parent = 0);
		~ConceptNetGui();

		/**
		 * Get current solver settings
		 * @return shared_ptr<SolverSettings>
		 */
		shared_ptr<SolverSettings> getSettings();
		/**
		 * Set current solver settings
		 * @param settings shared_ptr<SolverSettings> settings to be given to the solver
		 */
		void setSettings(shared_ptr<SolverSettings> settings);
		/**
		 * Gets the pointer to the QT main window
		 * @return Ui::ConceptNetGui*
		 */
		Ui::ConceptNetGui* getUi();
		/**
		 * History of abstract Command class
		 * Used to save the order of commands
		 * Undo via ctrl+z
		 */
		vector<shared_ptr<Command>> commandHistory;
		/**
		 * Pointer to CommandHistoryHandler
		 * Handles interaction with the command history
		 */
		CommandHistoryHandler* chHandler;
		/**
		 * Pointer to the SaveLoadHandler
		 * Handles Save and Load operations
		 */
		SaveLoadHandler* slHandler;
		/**
		 * Pointer to SettingsDialog class
		 * Used to change Solver parameters
		 */
		SettingsDialog* settingsDialog;
		/**
		 * Enables/Disables Gui elements
		 * @param enable bool true to enable
		 */
		void enableGui(bool enable);
		/**
		 * Clears Gui labels
		 */
		void clear();

		reasoner::ASPSolver* getSolver();

		void setSolver(reasoner::ASPSolver* solver);

	private slots:
		/**
		 * Slot to create new Solver
		 */
		void newSolver();

		/**
		 * Slot for Ground call
		 */
		void groundCallBack();
		/**
		 * Slot for Solve call
		 */
		void solveCallBack();
		/**
		 * Slot for Query call
		 */
		void queryCallBack();
		/**
		 * Slot for ConceptNet call
		 */
		void conceptNetCallBack();

	private:

		bool checkDockerInstallation();

		bool checkConcneptNetInstallation();

		bool rulesAreDotTerminated();

		reasoner::ASPSolver* solver;

		/**
		 * Current Settings
		 */
		shared_ptr<SolverSettings> settings;
		/**
		 * Pointer to main window
		 */
		Ui::ConceptNetGui* ui;
		/**
		 * Pointer to new Solver Dialog
		 */
		QMessageBox* msgBox;
		/**
		 * Shortcut to ctrl+z
		 */
		QShortcut* strgZShortcut;
		/**
		 * Shortcut to esc button
		 */
		QShortcut* escShortcut;

		/**
		 * Connect Gui elements to corresponding slots
		 */
		void connectGuiElements();

		bool isDockerInstalled;
		bool isConcneptNetInstalled;

	signals:
		/**
		 * Signal used to redraw the command history
		 */
		void updateCommandList();
	};
}
#endif // CONCEPTNETGUI_H

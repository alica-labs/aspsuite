#ifndef KnowledgebaseCreator_H
#define KnowledgebaseCreator_H

#include <QMainWindow>
#include <QtGui>
#include <QMessageBox>
#include <QShortcut>

#include <iostream>
#include <memory>

#include <clingo.hh>

//#define GUIDEUG

namespace reasoner
{
	class ASPSolver;
}

namespace Ui
{
	class KnowledgebaseCreator;
}

namespace kbcr
{
	class SolverSettings;
	class SettingsDialog;
	class ModelSettingDialog;
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
	class ExternalTableHandler;
	class ConceptNetCall;
	/**
	 * Class containing the main window and managing the connects, handler and provides the command history
	 */
	class KnowledgebaseCreator : public QMainWindow
	{
	Q_OBJECT

	public:
		/**
		 * Constructor
		 * @param parent pointer to main widget
		 */
		explicit KnowledgebaseCreator(QWidget *parent = 0);
		~KnowledgebaseCreator();

		/**
		 * Get current solver settings
		 * @return shared_ptr<SolverSettings>
		 */
		std::shared_ptr<SolverSettings> getSettings();
		/**
		 * Set current solver settings
		 * @param settings shared_ptr<SolverSettings> settings to be given to the solver
		 */
		void setSettings(std::shared_ptr<SolverSettings> settings);
		/**
		 * Gets the pointer to the QT main window
		 * @return Ui::KnowledgebaseCreator*
		 */
		Ui::KnowledgebaseCreator* getUi();
		/**
		 * History of abstract Command class
		 * Used to save the order of commands
		 * Undo via ctrl+z
		 */
		std::vector<std::shared_ptr<Command>> commandHistory;
		/**
		 * Pointer to CommandHistoryHandler
		 * Handles interaction with the command history
		 */
		CommandHistoryHandler* chHandler;
		/**
		 * Pointer to the SaveLoadHandler
		 * Handles filling the external table
		 */
		SaveLoadHandler* slHandler;
		/**
		 * Pointer to the ExternalTableHandler
		 * Handles Save and Load operations
		 */
		ExternalTableHandler* esHandler;
		/**
		 * Pointer to SettingsDialog class
		 * Used to change Solver parameters
		 */
		SettingsDialog* settingsDialog;

		ModelSettingDialog* modelSettingsDialog;

		/**
		 * Enables/Disables Gui elements
		 * @param enable bool true to enable
		 */
		void enableGui(bool enable);
		/**
		 * Clears Gui labels
		 */
		void clear();
		/**
		 * Get Solver pointer
		 */
		reasoner::ASPSolver* getSolver();
		/**
		 * Set solver Pointer
		 */
		void setSolver(reasoner::ASPSolver* solver);
		/**
		 * Get list of basic cn5 relations
		 */
		std::vector<QString> getConceptNetBaseRealtions();
		/**
		 * Containts the begin of a concept net query url.
		 */
		static const QString CONCEPTNET_BASE_URL;
		/**
		 * Query part: /query?start=/c/en/
		 */
        static const QString CONCEPTNET_URL_QUERYPART;
		/**
		 * Containts the cn5_ prefix.
		 */
		static const QString CONCEPTNET_PREFIX;

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
		/**
		 * Slot for Add call
		 */
		void addCallBack();
		/**
		 * Slot for ApplyExternal call
		 */
		void applyExternalCallBack();

	private:

		/**
		 * True if docker is installed
		 */
		bool checkDockerInstallation();
		/**
		 * True if the concept net database is available locally
		 */
		bool checkConcneptNetInstallation();
		/**
		 * Checks if all asp rules are dot terminated and informs the user if not
		 */
		bool rulesAreDotTerminated();
		/**
		 * Reads the concept net base relations given in a json file
		 */
		void readConceptNetBaseRelations();
		/**
		 * Solver pointer
		 */
		reasoner::ASPSolver* solver;
		/**
		 * Current Settings
		 */
		std::shared_ptr<SolverSettings> settings;
		/**
		 * Pointer to main window
		 */
		Ui::KnowledgebaseCreator* ui;
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
		/**
		 * Set auto wrap on labels and enable selection with mouse
		 */
		void configureLabels();
		/**
		 * true if docker is installed
		 */
//		bool isDockerInstalled;
		/**
		 * true if concept net is available
		 */
//		bool isConcneptNetInstalled;
		/**
		 * contains base relations
		 */
		std::vector<QString> conceptNetBaseRealtions;

	signals:
		/**
		 * Signal used to redraw the command history
		 */
		void updateCommandList();
		/**
		 * Signal used to redraw the ExternalList
		 */
		void updateExternalList();
	};
}
#endif // KnowledgebaseCreator_H

#ifndef CONCEPTNETGUI_H
#define CONCEPTNETGUI_H

#include <QMainWindow>
#include <QtGui>
#include <QMessageBox>
#include <QShortcut>

#include <iostream>
#include <memory>

using namespace std;

namespace supplementary
{
	class SystemConfig;
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
	class ConceptNetGui : public QMainWindow
	{
	Q_OBJECT

	public:
		explicit ConceptNetGui(QWidget *parent = 0);
		~ConceptNetGui();
		void addToCommandHistory(shared_ptr<Command> cmd);
		void removeFromCommandHistory(shared_ptr<Command> cmd);

		shared_ptr<SolverSettings> getSettings();
		void setSettings(shared_ptr<SolverSettings> settings);
		Ui::ConceptNetGui* getUi();

		void enableGui(bool enable);
		void clear();

	private slots:
		// menu solts
		void newSolver();
		void saveProgram();
		void saveModels();
		void loadProgram();
		void loadBackgroundKnowledge();

		// button slots
		void groundCallBack();
		void solveCallBack();
		void queryCallBack();
		void conceptNetCallBack();

		// command history
		void fillCommandHistory();
		void drawHistoryTable();
		void undoHistory();
		void removeFocus();

	private:
		shared_ptr<SolverSettings> settings;
		Ui::ConceptNetGui* ui;
		SettingsDialog* settingsDialog;
		QMessageBox* msgBox;
		supplementary::SystemConfig* sc;
		vector<shared_ptr<Command>> commandHistory;
		QShortcut* strgZShortcut;
		QShortcut* escShortcut;

		void connectGuiElements();

		void addNewSolverCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addChangeSolverSettingsCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addConceptNetQueryCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addGroundCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addLoadSavedProgramCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addSolveCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addVariableQueryCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addLoadBackgroundKnowledgeCommandToHistory(shared_ptr<Command> cmd, int pos);
		void addFactsQueryCommandToHistory(shared_ptr<Command> cmd, int pos);

	signals:
		void updateCommandList();
	};
}
#endif // CONCEPTNETGUI_H

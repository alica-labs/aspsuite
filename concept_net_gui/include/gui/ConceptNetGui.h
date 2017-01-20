#ifndef CONCEPTNETGUI_H
#define CONCEPTNETGUI_H

#include "../include/commands/Command.h"

#include <QMainWindow>
#include <QtGui>

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
	class ConceptNetGui : public QMainWindow
	{
	Q_OBJECT

	public:
		explicit ConceptNetGui(QWidget *parent = 0);
		~ConceptNetGui();
		void addToCommandHistory(shared_ptr<Command> cmd);

		shared_ptr<SolverSettings> getSettings();
		void setSettings(shared_ptr<SolverSettings> settings);

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
		void fillCommandList();
		void drawHistoryTable();

	private:
		shared_ptr<SolverSettings> settings;
		Ui::ConceptNetGui* ui;
		SettingsDialog* settingsDialog;
		NewSolverDialog* newSolverDialog;
		supplementary::SystemConfig* sc;
		vector<shared_ptr<Command>> commandHistory;

	signals:
		void updateCommandList();

	};
}
#endif // CONCEPTNETGUI_H

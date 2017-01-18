#ifndef CONCEPTNETGUI_H
#define CONCEPTNETGUI_H

#include <QMainWindow>
#include <QtGui>
#include <iostream>
#include "../include/Command.h"

using namespace std;

namespace supplementary
{
	class SystemConfig;
}

namespace Ui
{
	class ConceptNetGui;
}

class SettingsDialog;
class ConceptNetGui : public QMainWindow
{
Q_OBJECT

public:
	explicit ConceptNetGui(QWidget *parent = 0);
	~ConceptNetGui();

private slots:
	// menu solts
	void newSolver();
	void saveProgram();
	void saveModels();
	void loadModels();
	void loadProgram();
	void loadBackgroundKnowledge();

	// settings slots
	void applySettings();

	// button slots
	void groundCallBack();
	void solveCallBack();
	void queryCallBack();
	void conceptNetCallBack();

private:
	vector<char const*> arguments;
	Ui::ConceptNetGui* ui;
	SettingsDialog* settingsDialog;
	supplementary::SystemConfig* sc;
	vector<Command> commandHistory;
};

#endif // CONCEPTNETGUI_H

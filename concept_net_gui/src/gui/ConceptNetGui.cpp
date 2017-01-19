#include "../include/gui/ConceptNetGui.h"
#include "../include/gui/SettingsDialog.h"
#include "../include/gui/NewSolverDialog.h"
#include "../include/commands/NewSolverCommand.h"
#include "../include/commands/ChangeSolverSettingsCommand.h"

#include <ui_conceptnetgui.h>
#include <ui_settingsdialog.h>

#include <QUrl>
#include <qfiledialog.h>
#include <qdebug.h>

#include <SystemConfig.h>

namespace cng
{
	ConceptNetGui::ConceptNetGui(QWidget *parent) :
			QMainWindow(parent), ui(new Ui::ConceptNetGui)
	{
		this->settingsDialog = new SettingsDialog(parent, this);
		this->newSolverDialog = new NewSolverDialog(parent, this);
		this->ui->setupUi(this);
		drawHistoryTable();

		// Settings connects
		this->connect(this->ui->actionSolver_Settings, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));

		// Menu connects
		this->connect(this->ui->actionSave_Program, SIGNAL(triggered()), this, SLOT(saveProgram()));
		this->connect(this->ui->actionSave_Models, SIGNAL(triggered()), this, SLOT(saveModels()));
		this->connect(this->ui->actionLoad_Program, SIGNAL(triggered()), this, SLOT(loadProgram()));
		this->connect(this->ui->actionLoad_Background_Knowledge, SIGNAL(triggered()), this,
						SLOT(loadBackgroundKnowledge()));
		this->connect(this->ui->actionNew_Program, SIGNAL(triggered()), this, SLOT(newSolver()));

		// Button connects
		this->connect(this->ui->groundBtn, SIGNAL(released()), this, SLOT(groundCallBack()));
		this->connect(this->ui->solveBtn, SIGNAL(released()), this, SLOT(solveCallBack()));
		this->connect(this->ui->queryBtn, SIGNAL(released()), this, SLOT(queryCallBack()));
		this->connect(this->ui->conceptNetBtn, SIGNAL(released()), this, SLOT(conceptNetCallBack()));

		// Command History
		this->connect(this, SIGNAL(updateCommandList()), this, SLOT(fillCommandList()));

		this->sc = supplementary::SystemConfig::getInstance();
	}

	ConceptNetGui::~ConceptNetGui()
	{
		delete this->settingsDialog;
		delete this->newSolverDialog;
		delete this->ui;
	}

	void ConceptNetGui::newSolver()
	{
		if (this->arguments.size() == 0)
		{
			this->newSolverDialog->show();
		}
		else
		{
			shared_ptr<NewSolverCommand> cmd = make_shared<NewSolverCommand>(this->arguments, this, this->argumentString);
			this->addToCommandHistory(cmd);
			cmd->execute();
		}
	}

	void ConceptNetGui::saveProgram()
	{
		//Open save file dialog to save a program
		QString filename = QFileDialog::getSaveFileName(this->parentWidget(), tr("Save Programm"), QDir::currentPath(),
														tr("ConceptNetGui Program File (*.cnlp)"), 0,
														QFileDialog::DontUseNativeDialog);

		if (!filename.endsWith(".cnlp"))
		{
			filename += ".cnlp";
		}

		//Check if the user selected a correct file
		if (!filename.isNull())
		{

			//Create file
			QFile file(filename);

			if (!file.open(QIODevice::WriteOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

		}

		//TODO JSON serialization of command history
	}

	void ConceptNetGui::saveModels()
	{
		//Open save file dialog to save models
		QString filename = QFileDialog::getSaveFileName(this->parentWidget(), tr("Save Current Models"),
														QDir::currentPath(), tr("ConceptNetGui Models File (*.cnmod)"),
														0, QFileDialog::DontUseNativeDialog);

		if (!filename.endsWith(".cnmod"))
		{
			filename += ".cnmod";
		}

		//Check if the user selected a correct file
		if (!filename.isNull())
		{

			//Create file
			QFile file(filename);

			if (!file.open(QIODevice::WriteOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

		}

		//TODO JSON serialization of models
	}

	void ConceptNetGui::loadProgram()
	{
		//Open load file dialog to select a pregenerated wumpus world
		QString filename = QFileDialog::getOpenFileName(this->parentWidget(), tr("Load Program"), QDir::currentPath(),
														tr("ConceptNetGui Program File (*.cnlp)"), 0,
														QFileDialog::DontUseNativeDialog);

		//Check if the user selected a correct file
		if (!filename.isNull())
		{
			//Open file
			QFile file(filename);
			if (!file.open(QIODevice::ReadOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

			QByteArray saveData = file.readAll();
		}
	}

	void ConceptNetGui::loadBackgroundKnowledge()
	{
		//Open load file dialog to select a pregenerated wumpus world
		QString filename = QFileDialog::getOpenFileName(this->parentWidget(), tr("Load Background Knowledge"),
														QDir::currentPath(), tr("Logic Program (*.lp)"), 0,
														QFileDialog::DontUseNativeDialog);

		//Check if the user selected a correct file
		if (!filename.isNull())
		{
			//Open file
			QFile file(filename);
			if (!file.open(QIODevice::ReadOnly))
			{
				qWarning("Couldn't open file.");
				return;
			}

			QByteArray saveData = file.readAll();
			this->ui->aspRuleTextArea->setText(saveData);
		}
	}

	void ConceptNetGui::groundCallBack()
	{
		cout << "gound" << endl;
		//TODO call ground method
	}

	void ConceptNetGui::solveCallBack()
	{
		cout << "solve" << endl;
		//TODO call solve method
	}

	void ConceptNetGui::queryCallBack()
	{
		cout << "query" << endl;
		//TODO create query
	}

	vector<const char*> ConceptNetGui::getArguments()
	{
		return this->arguments;
	}

	void ConceptNetGui::addToCommandHistory(shared_ptr<Command> cmd)
	{
		this->commandHistory.push_back(cmd);
		emit updateCommandList();
	}

	void ConceptNetGui::conceptNetCallBack()
	{
		cout << "conceptnet" << endl;
		//TODO ask conceptNet
	}

	void ConceptNetGui::setArguments(vector<const char*> arguments)
	{
		this->arguments = arguments;
	}

	void ConceptNetGui::drawHistoryTable()
	{
		this->ui->commandHistoryTable->clear();
		this->ui->commandHistoryTable->setColumnCount(2);
		this->ui->commandHistoryTable->setHorizontalHeaderLabels(QStringList {"Command", "Parameters", NULL});
		this->ui->commandHistoryTable->setColumnWidth(0, 200);
		this->ui->commandHistoryTable->setColumnWidth(
				1, this->ui->commandHistoryTable->width() - this->ui->commandHistoryTable->columnWidth(0));
		this->ui->commandHistoryTable->setRowCount(0);
	}

	void ConceptNetGui::fillCommandList()
	{
		drawHistoryTable();
		for (auto cmd : this->commandHistory)
		{
			int pos = this->ui->commandHistoryTable->rowCount();
			this->ui->commandHistoryTable->insertRow(pos);

			auto tmp = new QTableWidgetItem(QString(cmd->getType().c_str()));
			tmp->setFlags(tmp->flags() ^ Qt::ItemIsEditable);
			this->ui->commandHistoryTable->setItem(pos, 0, tmp);

			if (dynamic_pointer_cast<NewSolverCommand>(cmd))
			{
				auto tmp2 = new QTableWidgetItem(QString(dynamic_pointer_cast<NewSolverCommand>(cmd)->argumentString.c_str()));
				tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
				this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
			}

			if (dynamic_pointer_cast<ChangeSolverSettingsCommand>(cmd))
			{
				auto tmp2 = new QTableWidgetItem(QString(dynamic_pointer_cast<ChangeSolverSettingsCommand>(cmd)->currentSettings.c_str()));
				tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
				this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
			}

		}
	}

	string ConceptNetGui::getArgumentString()
	{
		return argumentString;
	}

	void ConceptNetGui::setArgumentString(string argumentString)
	{
		this->argumentString = argumentString;
	}

}



#include "../include/gui/ConceptNetGui.h"
#include "../include/gui/SettingsDialog.h"

#include "../include/containers/SolverSettings.h"

#include "../include/commands/NewSolverCommand.h"
#include "../include/commands/ChangeSolverSettingsCommand.h"
#include "../include/commands/ConceptNetQueryCommand.h"
#include "../include/commands/Command.h"
#include "../include/commands/FactsQueryCommand.h"
#include "../include/commands/GroundCommand.h"
#include "../include/commands/LoadBackgroundKnowledgeCommand.h"
#include "../include/commands/LoadSavedProgramCommand.h"
#include "../include/commands/SolveCommand.h"
#include "../include/commands/VariableQueryCommand.h"

#include "../include/handler/CommandHistoryHandler.h"
#include "../include/handler/SaveLoadHandler.h"

#include <ui_conceptnetgui.h>
#include <ui_settingsdialog.h>

#include <QUrl>
#include <QFileDialog>
#include <QDebug>
#include <QScrollBar>
#include <QTextStream>

#include <SystemConfig.h>

namespace cng
{
	ConceptNetGui::ConceptNetGui(QWidget *parent) :
			QMainWindow(parent), ui(new Ui::ConceptNetGui)
	{
		this->ui->setupUi(this);
		this->ui->resultTabWidget->setCurrentIndex(0);

		this->settingsDialog = new SettingsDialog(parent, this);
		this->msgBox = new QMessageBox();
		this->strgZShortcut = new QShortcut(QKeySequence(QKeySequence::Undo), this->ui->mainWindow);
		this->escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this->ui->mainWindow);
		this->chHandler =  new CommandHistoryHandler(this);
		this->slHandler = new SaveLoadHandler(this);

		this->sc = supplementary::SystemConfig::getInstance();
		this->settings = nullptr;
		this->enableGui(false);
		this->connectGuiElements();
	}

	ConceptNetGui::~ConceptNetGui()
	{
		delete this->chHandler;
		delete this->slHandler;
		delete this->strgZShortcut;
		delete this->escShortcut;
		delete this->settingsDialog;
		delete this->msgBox;
		delete this->ui;
	}

	void ConceptNetGui::newSolver()
	{
		if (this->settings == nullptr)
		{
			msgBox->setWindowTitle("New Solver");
			msgBox->setText("No Solver Settings given!");
			msgBox->setInformativeText("Create Solver with default Settings?");
			msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			msgBox->setDefaultButton(QMessageBox::Cancel);
			int btn = msgBox->exec();
			if (btn == QMessageBox::Ok)
			{
				shared_ptr<NewSolverCommand> cmd = make_shared<NewSolverCommand>(this, this->settings);
				cmd->execute();
			}
		}
		else
		{
			shared_ptr<NewSolverCommand> cmd = make_shared<NewSolverCommand>(this, this->settings);
			cmd->execute();
		}
	}

	void ConceptNetGui::groundCallBack()
	{
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty())
		{
			return;
		}
		cout << "ConceptNetGui: gound" << endl;
		shared_ptr<GroundCommand> cmd = make_shared<GroundCommand>(this, this->ui->aspRuleTextArea->toPlainText());
		cmd->execute();
	}

	void ConceptNetGui::solveCallBack()
	{
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty())
		{
			return;
		}
		cout << "ConceptNetGui: solve" << endl;
		shared_ptr<SolveCommand> cmd = make_shared<SolveCommand>(this);
		cmd->execute();
	}

	void ConceptNetGui::queryCallBack()
	{
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty())
		{
			return;
		}
		cout << "ConceptNetGui: query" << endl;
		if (this->ui->aspRuleTextArea->toPlainText().contains(QString("wildcard")))
		{
			shared_ptr<FactsQueryCommand> cmd = make_shared<FactsQueryCommand>(this);
			cmd->execute();
		}
		else
		{
			shared_ptr<VariableQueryCommand> cmd = make_shared<VariableQueryCommand>(this);
			cmd->execute();
		}
	}

	void ConceptNetGui::conceptNetCallBack()
	{
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty())
		{
			return;
		}
		cout << "ConceptNetGui: conceptnet" << endl;
		shared_ptr<ConceptNetQueryCommand> cmd = make_shared<ConceptNetQueryCommand>(this);
		cmd->execute();
	}

	void ConceptNetGui::enableGui(bool enable)
	{
		this->ui->actionLoad_Background_Knowledge->setEnabled(enable);
//		this->ui->actionLoad_Program->setEnabled(enable);
		this->ui->actionSave_Models->setEnabled(enable);
		this->ui->actionSave_Program->setEnabled(enable);
		this->ui->aspRuleTextArea->setEnabled(enable);
		this->ui->conceptNetBtn->setEnabled(enable);
		this->ui->groundBtn->setEnabled(enable);
		this->ui->solveBtn->setEnabled(enable);
		this->ui->queryBtn->setEnabled(enable);
		this->ui->resultTabWidget->setEnabled(enable);
		this->strgZShortcut->setEnabled(enable);
	}

	void ConceptNetGui::connectGuiElements()
	{
		// Settings connects
		this->connect(this->ui->actionSolver_Settings, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));

		// Menu connects
		this->connect(this->ui->actionSave_Program, SIGNAL(triggered()), this->slHandler, SLOT(saveProgram()));
		this->connect(this->ui->actionSave_Models, SIGNAL(triggered()), this->slHandler, SLOT(saveModels()));
		this->connect(this->ui->actionLoad_Program, SIGNAL(triggered()), this->slHandler, SLOT(loadProgram()));
		this->connect(this->ui->actionLoad_Background_Knowledge, SIGNAL(triggered()), this->slHandler,
						SLOT(loadBackgroundKnowledge()));
		this->connect(this->ui->actionNew_Program, SIGNAL(triggered()), this, SLOT(newSolver()));

		// Button connects
		this->connect(this->ui->groundBtn, SIGNAL(released()), this, SLOT(groundCallBack()));
		this->connect(this->ui->solveBtn, SIGNAL(released()), this, SLOT(solveCallBack()));
		this->connect(this->ui->queryBtn, SIGNAL(released()), this, SLOT(queryCallBack()));
		this->connect(this->ui->conceptNetBtn, SIGNAL(released()), this, SLOT(conceptNetCallBack()));

		// Command History
		this->connect(this, SIGNAL(updateCommandList()), this->chHandler, SLOT(fillCommandHistory()));
		this->connect(strgZShortcut, SIGNAL(activated()), this->chHandler, SLOT(undoHistory()));
		this->connect(escShortcut, SIGNAL(activated()), this->chHandler, SLOT(removeFocus()));
	}

	void ConceptNetGui::clear()
	{
		this->ui->currentModelsLabel->clear();
		this->ui->externalStatementsTable->clear();
		this->ui->sortedModelsLabel->clear();
		this->ui->queryResultsLabel->clear();
		this->ui->aspRuleTextArea->clear();
	}

	void ConceptNetGui::setSettings(shared_ptr<SolverSettings> settings)
	{
		this->settings = settings;
	}

	Ui::ConceptNetGui* ConceptNetGui::getUi()
	{
		return ui;
	}

	shared_ptr<SolverSettings> ConceptNetGui::getSettings()
	{
		return settings;
	}
}


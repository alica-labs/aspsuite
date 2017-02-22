#include "gui/ConceptNetGui.h"
#include "gui/SettingsDialog.h"

#include "containers/SolverSettings.h"

#include "commands/NewSolverCommand.h"
#include "commands/ChangeSolverSettingsCommand.h"
#include "commands/ConceptNetQueryCommand.h"
#include "commands/Command.h"
#include "commands/FactsQueryCommand.h"
#include "commands/GroundCommand.h"
#include "commands/LoadBackgroundKnowledgeCommand.h"
#include "commands/LoadSavedProgramCommand.h"
#include "commands/SolveCommand.h"
#include "commands/VariableQueryCommand.h"

#include "handler/CommandHistoryHandler.h"
#include "handler/SaveLoadHandler.h"

#include <ui_conceptnetgui.h>
#include <ui_settingsdialog.h>

#include <QUrl>
#include <QFileDialog>
#include <QDebug>
#include <QScrollBar>
#include <QTextStream>

#include <asp_commons/IASPSolver.h>
#include <asp_solver/ASPSolver.h>

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
		this->chHandler = new CommandHistoryHandler(this);
		this->slHandler = new SaveLoadHandler(this);

		this->settings = nullptr;
		this->solver = nullptr;
		this->enableGui(false);
		this->ui->currentModelsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		this->ui->currentModelsLabel->setWordWrap(true);
		this->ui->queryResultsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		this->ui->queryResultsLabel->setWordWrap(true);
		this->ui->sortedModelsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		this->ui->sortedModelsLabel->setWordWrap(true);
		this->ui->showModelsCheckBox->setChecked(true);
		this->connectGuiElements();
		this->isDockerInstalled = checkDockerInstallation();
		this->isConcneptNetInstalled = checkConcneptNetInstallation();
		if(isDockerInstalled && isConcneptNetInstalled)
		{
			int i = system("docker start conceptnet5_conceptnet-web_1");
			cout << "ConceptNetGui: \"docker start conceptnet5_conceptnet-web_1\" was called with exit code: " << i << endl;
		}
	}

	ConceptNetGui::~ConceptNetGui()
	{
		if(isDockerInstalled && isConcneptNetInstalled)
		{
			int i = system("docker stop conceptnet5_conceptnet-web_1");
			cout << "ConceptNetGui: \"docker stop conceptnet5_conceptnet-web_1\" was called with exit code: " << i << endl;
		}
		if (this->solver != nullptr)
		{
			delete this->solver;
		}
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
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty() || !rulesAreDotTerminated())
		{
			return;
		}
		shared_ptr<GroundCommand> cmd = make_shared<GroundCommand>(this, this->ui->aspRuleTextArea->toPlainText());
		cmd->execute();
	}

	void ConceptNetGui::solveCallBack()
	{
//		if (this->ui->aspRuleTextArea->toPlainText().isEmpty())
//		{
//			return;
//		}
		shared_ptr<SolveCommand> cmd = make_shared<SolveCommand>(this);
		cmd->execute();
	}

	void ConceptNetGui::queryCallBack()
	{
		if (this->ui->aspRuleTextArea->toPlainText().isEmpty() || !rulesAreDotTerminated())
		{
			return;
		}
		if (this->ui->aspRuleTextArea->toPlainText().contains(QString("wildcard")))
		{
			shared_ptr<FactsQueryCommand> cmd = make_shared<FactsQueryCommand>(this,
																				ui->aspRuleTextArea->toPlainText());
			cmd->execute();
		}
		else
		{
			shared_ptr<VariableQueryCommand> cmd = make_shared<VariableQueryCommand>(
					this, ui->aspRuleTextArea->toPlainText());
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
		this->ui->actionSave_Models->setEnabled(enable);
		this->ui->actionSave_Program->setEnabled(enable);
		this->ui->aspRuleTextArea->setEnabled(enable);
		if (!enable)
		{
			this->ui->conceptNetBtn->setEnabled(enable);
		}

		else
		{
			if(isDockerInstalled && isConcneptNetInstalled)
			{
				this->ui->conceptNetBtn->setEnabled(enable);
			}
		}
		this->ui->groundBtn->setEnabled(enable);
		this->ui->solveBtn->setEnabled(enable);
		this->ui->queryBtn->setEnabled(enable);
		this->ui->resultTabWidget->setEnabled(enable);
		this->strgZShortcut->setEnabled(enable);
		this->ui->numberOfModelsSpinBox->setEnabled(enable);
		this->ui->numberOfmodelsLabel->setEnabled(enable);
		this->ui->showModelsCheckBox->setEnabled(enable);
	}

	bool ConceptNetGui::checkDockerInstallation()
	{
		std::array<char, 128> buffer;
		std::string result;
		std::shared_ptr<FILE> pipe(popen("docker -v", "r"), pclose);
		if (!pipe)
			throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get()))
		{
			if (fgets(buffer.data(), 128, pipe.get()) != NULL)
				result += buffer.data();
		}
#ifdef GUIDEUG
		cout << "ConceptNetGui: " << result << endl;
#endif
		return (result.find("Docker version ") != string::npos);
	}

	bool ConceptNetGui::checkConcneptNetInstallation()
	{
		std::array<char, 128> buffer;
		std::string result;
		std::shared_ptr<FILE> pipe(popen("docker ps -a", "r"), pclose);
		if (!pipe)
			throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get()))
		{
			if (fgets(buffer.data(), 128, pipe.get()) != NULL)
				result += buffer.data();
		}
#ifdef GUIDEUG
		cout << "ConceptNetGui: " << result << endl;
#endif
		return (result.find("conceptnet5_conceptnet-web_1") != string::npos);
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

	reasoner::ASPSolver* ConceptNetGui::getSolver()
	{
		return solver;
	}

	void ConceptNetGui::setSolver(reasoner::ASPSolver* solver)
	{
		this->solver = solver;
	}

	bool ConceptNetGui::rulesAreDotTerminated()
	{
		auto program = this->ui->aspRuleTextArea->toPlainText().toStdString();
		std::string delimiter = "\n";

		size_t pos = 0;
		string token;
		vector<string> lines;
		while ((pos = program.find(delimiter)) != string::npos)
		{
			token = program.substr(0, pos);
			lines.push_back(token);
			program.erase(0, pos + delimiter.length());
		}
		lines.push_back(program);
		for (auto line : lines)
		{
			if (!(line.empty() || line.find("%") != string::npos))
			{
				if (line.find(".") == string::npos)
				{
					QMessageBox mBox;
					mBox.setWindowTitle("Rule not terminated!");
					mBox.setText("The following rule is not terminated by a dot:");
					mBox.setInformativeText(QString(line.c_str()));
					mBox.setStandardButtons(QMessageBox::Ok);
					mBox.setDefaultButton(QMessageBox::Ok);
					mBox.exec();
					return false;
				}
			}
		}
		return true;
	}

}

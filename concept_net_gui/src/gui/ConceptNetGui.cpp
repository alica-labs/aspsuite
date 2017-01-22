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
		this->settingsDialog = new SettingsDialog(parent, this);
		this->msgBox = new QMessageBox();
		this->ui->setupUi(this);
		this->ui->resultTabWidget->setCurrentIndex(0);
		this->strgZShortcut = new QShortcut(QKeySequence(QKeySequence::Undo), this->ui->mainWindow);
		this->escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this->ui->mainWindow);

		this->connectGuiElements();

		this->sc = supplementary::SystemConfig::getInstance();
		this->settings = nullptr;
		this->enableGui(false);
	}

	ConceptNetGui::~ConceptNetGui()
	{
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

			QJsonObject history;

			//JSON Array to hold the commandHistory
			QJsonArray jsonCommandHistory;
			for (auto cmd : this->commandHistory)
			{
				//Skip load saved commandHistory to avoid recursive loading
				if(cmd->getType().compare("Load Saved Program") == 0)
				{
					continue;
				}
				jsonCommandHistory.append(cmd->toJSON());
			}
			history["commandHistory"] = jsonCommandHistory;
			//Write to file
			QJsonDocument saveDoc(history);
			file.write(saveDoc.toJson());
		}
	}

	void ConceptNetGui::saveModels()
	{
		//Open save file dialog to save models
		QString filename = QFileDialog::getSaveFileName(this->parentWidget(), tr("Save Current Models"),
														QDir::currentPath(), tr("ConceptNetGui Models File (*.txt)"), 0,
														QFileDialog::DontUseNativeDialog);

		if (!filename.endsWith(".txt"))
		{
			filename += ".txt";
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

			QString models;
			if (this->settingsDialog->isSaveSortedChecked())
			{
				models = this->ui->sortedModelsLabel->text();
			}
			else
			{
				models = this->ui->currentModelsLabel->text();
			}
			// Point a QTextStream object at the file
			QTextStream outStream(&file);

			// Write the line to the file
			outStream << models;

			// Close the file
			file.close();
		}
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

			QByteArray loadedData = file.readAll();
			shared_ptr<LoadSavedProgramCommand> cmd = make_shared<LoadSavedProgramCommand>(this, loadedData);
			cmd->execute();
		}
	}

	void ConceptNetGui::loadBackgroundKnowledge()
	{
		//Open load file dialog to select a pregenerated wumpus world
		this->ui->aspRuleTextArea->clear();

		QString filename = QFileDialog::getOpenFileName(this->parentWidget(), tr("Load Background Knowledge"),
														QDir::currentPath(), tr("Logic Program (*.lp)"), 0,
														QFileDialog::DontUseNativeDialog);
		//Check if the user selected a correct file
		if (!filename.isNull())
		{
			shared_ptr<LoadBackgroundKnowledgeCommand> cmd = make_shared<LoadBackgroundKnowledgeCommand>(this, filename);
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

	void ConceptNetGui::addToCommandHistory(shared_ptr<Command> cmd)
	{
		this->commandHistory.push_back(cmd);
		emit updateCommandList();
	}

	void ConceptNetGui::removeFromCommandHistory(shared_ptr<Command> cmd)
	{
		auto it = std::find_if(this->commandHistory.begin(), this->commandHistory.end(),
								[cmd](std::shared_ptr<Command> const& i)
								{	return i.get() == cmd.get();});
		if (it != this->commandHistory.end())
		{
			this->commandHistory.erase(it);
		}

		emit updateCommandList();
	}

	void ConceptNetGui::drawHistoryTable()
	{
		this->ui->commandHistoryTable->clear();
		this->ui->commandHistoryTable->setColumnCount(2);
		this->ui->commandHistoryTable->setHorizontalHeaderLabels(QStringList {"Command", "Parameters", NULL});
		this->ui->commandHistoryTable->setColumnWidth(0, 200);
		this->ui->commandHistoryTable->horizontalHeader()->stretchLastSection();
		this->ui->commandHistoryTable->setRowCount(0);
	}

	void ConceptNetGui::undoHistory()
	{
		auto lastIndex = this->commandHistory.size() - 1;
		auto cmd = this->commandHistory.at(lastIndex);
		cout << "ConceptNetGui: undo: " << cmd->getType() << endl;
		cmd->undo();
	}

	void ConceptNetGui::removeFocus()
	{
		this->ui->aspRuleTextArea->clearFocus();
	}

	void ConceptNetGui::fillCommandHistory()
	{
		drawHistoryTable();
		for (auto cmd : this->commandHistory)
		{
			int pos = this->ui->commandHistoryTable->rowCount();
			this->ui->commandHistoryTable->insertRow(pos);

			auto tmp = new QTableWidgetItem(QString(cmd->getType().c_str()));
			tmp->setFlags(tmp->flags() ^ Qt::ItemIsEditable);
			this->ui->commandHistoryTable->setItem(pos, 0, tmp);

			if (cmd->getType().compare("New Solver") == 0)
			{
				this->addNewSolverCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Change Settings") == 0)
			{
				this->addChangeSolverSettingsCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Concept Net") == 0)
			{
				this->addConceptNetQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Facts Query") == 0)
			{
				this->addFactsQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Ground") == 0)
			{
				this->addGroundCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Load Logic Program") == 0)
			{
				this->addLoadBackgroundKnowledgeCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Load Saved Program") == 0)
			{
				this->addLoadSavedProgramCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Variable Query") == 0)
			{
				this->addVariableQueryCommandToHistory(cmd, pos);
				continue;
			}
			else if (cmd->getType().compare("Solve") == 0)
			{
				this->addSolveCommandToHistory(cmd, pos);
				continue;
			}
			else
			{
				cout << "ConceptNetGui: Command with unknown type found!" << endl;
				auto tmp2 = new QTableWidgetItem(QString("Command with unknown type found!"));
				tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
				this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
			}
		}
		this->ui->commandHistoryTable->scrollToBottom();
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
		this->connect(this, SIGNAL(updateCommandList()), this, SLOT(fillCommandHistory()));
		this->connect(strgZShortcut, SIGNAL(activated()), this, SLOT(undoHistory()));
		this->connect(escShortcut, SIGNAL(activated()), this, SLOT(removeFocus()));
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

	void ConceptNetGui::addNewSolverCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<NewSolverCommand>(cmd)->settings->argString.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addChangeSolverSettingsCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<ChangeSolverSettingsCommand>(cmd)->currentSettings->argString.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addConceptNetQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addGroundCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(
				QString(dynamic_pointer_cast<GroundCommand>(cmd)->historyProgramSection.c_str()));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addLoadSavedProgramCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addSolveCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addVariableQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addLoadBackgroundKnowledgeCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString(dynamic_pointer_cast<LoadBackgroundKnowledgeCommand>(cmd)->fileName));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

	void ConceptNetGui::addFactsQueryCommandToHistory(shared_ptr<Command> cmd, int pos)
	{
		auto tmp2 = new QTableWidgetItem(QString("TODO"));
		tmp2->setFlags(tmp2->flags() ^ Qt::ItemIsEditable);
		this->ui->commandHistoryTable->setItem(pos, 1, tmp2);
	}

}


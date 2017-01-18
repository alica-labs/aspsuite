#include "../include/ConceptNetGui.h"
#include <ui_conceptnetgui.h>
#include "../include/SettingsDialog.h"
#include <QUrl>
#include <qfiledialog.h>
#include <qdebug.h>
#include <ui_settingsdialog.h>
#include <SystemConfig.h>

ConceptNetGui::ConceptNetGui(QWidget *parent) :
		QMainWindow(parent), ui(new Ui::ConceptNetGui)
{
	this->settingsDialog = new SettingsDialog(parent);
	this->ui->setupUi(this);

	// Settings connects
	this->connect(this->ui->actionSolver_Settings, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));

	// Menu connects
	this->connect(this->ui->actionSave_Program, SIGNAL(triggered()), this, SLOT(saveProgram()));
	this->connect(this->ui->actionSave_Models, SIGNAL(triggered()), this, SLOT(saveModels()));
	this->connect(this->ui->actionLoad_Program, SIGNAL(triggered()), this, SLOT(loadProgram()));
	this->connect(this->ui->actionLoad_Models, SIGNAL(triggered()), this, SLOT(loadModels()));
	this->connect(this->ui->actionLoad_Background_Knowledge, SIGNAL(triggered()), this,
					SLOT(loadBackgroundKnowledge()));
	this->connect(this->ui->actionNew_Program, SIGNAL(triggered()), this, SLOT(newSolver()));

	// Button connects
	this->connect(this->ui->groundBtn, SIGNAL(released()), this, SLOT(groundCallBack()));
	this->connect(this->ui->solveBtn, SIGNAL(released()), this, SLOT(solveCallBack()));
	this->connect(this->ui->queryBtn, SIGNAL(released()), this, SLOT(queryCallBack()));
	this->connect(this->ui->conceptNetBtn, SIGNAL(released()), this, SLOT(conceptNetCallBack()));

	// Settings ok button connect
	this->connect(this->settingsDialog->getUi()->okBtn, SIGNAL(released()), this, SLOT(applySettings()));

	this->sc = supplementary::SystemConfig::getInstance();
}

ConceptNetGui::~ConceptNetGui()
{
	delete this->settingsDialog;
	delete this->ui;
}

void ConceptNetGui::newSolver()
{
	cout << "New solver clicked" << endl;
	//TODO instantiate new solver
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
													QDir::currentPath(), tr("ConceptNetGui Models File (*.cnmod)"), 0,
													QFileDialog::DontUseNativeDialog);

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

void ConceptNetGui::loadModels()
{
	//Open load file dialog to select a pregenerated wumpus world
	QString filename = QFileDialog::getOpenFileName(this->parentWidget(), tr("Load Models"), QDir::currentPath(),
													tr("ConceptNetGui Models File (*.cnmod)"), 0,
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

void ConceptNetGui::applySettings()
{
	cout << "ConceptNetGui: Solver Params: " << this->settingsDialog->getCurrentSettings() << endl;
	string params = this->settingsDialog->getCurrentSettings();
	this->arguments.clear();
	if (params.find(",") != string::npos)
	{
		size_t start = 0;
		size_t end = string::npos;
		string parsedParam = "";
		while (start != string::npos)
		{
			end = params.find(",", start);
			if (end == string::npos)
			{
				parsedParam = supplementary::Configuration::trim(params.substr(start, params.length() - start));
				this->arguments.push_back(parsedParam.c_str());
				break;
			}
			parsedParam = supplementary::Configuration::trim(params.substr(start, end - start));
			start = params.find(",", end);
			if (start != string::npos)
			{
				start += 1;
			}
			this->arguments.push_back(parsedParam.c_str());
		}
		this->arguments.push_back(nullptr);
	}
	else
	{
		this->arguments.push_back(params.c_str());
		this->arguments.push_back(nullptr);
	}
	//TODO pass params to solver
	this->settingsDialog->close();
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

void ConceptNetGui::conceptNetCallBack()
{
	cout << "conceptnet" << endl;
	//TODO ask conceptNet
}

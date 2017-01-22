/*
 * SettingsDialog.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#include "../include/gui/SettingsDialog.h"
#include "../include/gui/ConceptNetGui.h"
#include "../include/commands/ChangeSolverSettingsCommand.h"
#include "../include/containers/SolverSettings.h"

#include <ui_settingsdialog.h>

#include <iostream>
#include <memory>

#include <SystemConfig.h>

namespace cng
{
	SettingsDialog::SettingsDialog(QWidget *parent, ConceptNetGui* gui) :
			QDialog(parent), ui(new Ui::SettingsDialog)
	{
		this->mainGui = gui;
		this->ui->setupUi(this);
		this->connect(this->ui->cancelBtn, SIGNAL(released()), this, SLOT(close()));
		this->connect(this->ui->commandLineEdit, SIGNAL(textEdited(const QString &)), this,
						SLOT(setCurrentSettings(const QString &)));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemClicked(QListWidgetItem * )), this,
						SLOT(fillSettingsLabel(QListWidgetItem * )));
		this->connect(this->ui->okBtn, SIGNAL(released()), this, SLOT(applySettings()));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
						SLOT(fillSettingsLabel(QListWidgetItem *)));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
						SLOT(applySettings()));
		this->sc = supplementary::SystemConfig::getInstance();
		string tmp = (*this->sc)["ConceptNetGui"]->get<string>("ConceptNetGui", "saveSortedModels", NULL);
		bool checked;
		istringstream(tmp) >> std::boolalpha >> checked;
		this->ui->safeSortedModelscheckBox->setChecked(checked);
		loadSettingsFromConfig();
		fillSettingsList();
	}

	Ui::SettingsDialog* SettingsDialog::getUi()
	{
		return ui;
	}

	SettingsDialog::~SettingsDialog()
	{
		delete ui;
	}

	void SettingsDialog::setCurrentSettings(const QString& text)
	{
		this->currentSettings = text.toStdString();
	}

	void SettingsDialog::loadSettingsFromConfig()
	{
		const char* conceptNetGui = "ConceptNetGui";

		this->parameterSectionNames = (*this->sc)[conceptNetGui]->getSections(conceptNetGui, NULL);

		// Iterate over all model sections in config file
		for (auto section : *(this->parameterSectionNames))
		{
			this->parameterMap.emplace(
					section,
					make_shared<SolverSettings>(
							(*this->sc)[conceptNetGui]->get<string>(conceptNetGui, section.c_str(), "Parameters",
																	NULL)));
		}
	}

	void SettingsDialog::fillSettingsList()
	{
		for (auto pair : this->parameterMap)
		{
			this->ui->settingsListWidget->addItem(QString(pair.first.c_str()));
		}
	}

	void SettingsDialog::fillSettingsLabel(QListWidgetItem* item)
	{
		auto params = this->parameterMap.at(item->text().toStdString());
		this->currentSettings = params->argString;
		if (this->currentSettings.find(",") != string::npos)
		{
			string parameters = "";
			for (auto param : params->argumentStrings)
			{
				parameters += param;
				parameters += "\n";
			}
			this->ui->settingsLabel->setText(QString(parameters.c_str()));
		}
		else
		{
			this->ui->settingsLabel->setText(QString(this->currentSettings.c_str()));
		}
	}

	void SettingsDialog::applySettings()
	{

		if (this->currentSettings.empty())
		{
			this->close();
			return;
		}
		cout << "SettingsDialog: applying parameters: " << currentSettings << endl;
		shared_ptr<ChangeSolverSettingsCommand> cmd = make_shared<ChangeSolverSettingsCommand>(this->mainGui, this,
																								currentSettings);
		cmd->execute();
		this->close();
	}

	bool SettingsDialog::isSaveSortedChecked()
	{
		return this->ui->safeSortedModelscheckBox->isChecked();
	}

}

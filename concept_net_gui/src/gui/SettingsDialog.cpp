/*
 * SettingsDialog.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#include "../include/gui/SettingsDialog.h"
#include "../include/gui/ConceptNetGui.h"
#include "../include/commands/ChangeSolverSettingsCommand.h"

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
		this->connect(this->ui->okBtn, SIGNAL(released()), this,SLOT(applySettings()));
		this->sc = supplementary::SystemConfig::getInstance();
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

	string SettingsDialog::getCurrentSettings()
	{
		return currentSettings;
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
					(*this->sc)[conceptNetGui]->get<string>(conceptNetGui, section.c_str(), "Parameters", NULL));
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
		this->currentSettings = params;
		if (params.find(",") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string parsedParams = "";
			while (start != string::npos)
			{
				end = params.find(",", start);
				if (end == string::npos)
				{
					parsedParams += params.substr(start, params.length() - start);
					break;
				}
				parsedParams += params.substr(start, end - start);
				parsedParams += "\n";
				start = params.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
			}
			this->ui->settingsLabel->setText(QString(parsedParams.c_str()));
		}
		else
		{
			this->ui->settingsLabel->setText(QString(params.c_str()));
		}
	}

	void SettingsDialog::applySettings()
	{
		vector<char const*> arguments;
		cout << "SettingsDialog: applying parameters: " << currentSettings << endl;
		if (currentSettings.find(",") != string::npos)
		{
			size_t start = 0;
			size_t end = string::npos;
			string parsedParam = "";
			while (start != string::npos)
			{
				end = currentSettings.find(",", start);
				if (end == string::npos)
				{
					parsedParam = supplementary::Configuration::trim(currentSettings.substr(start, currentSettings.length() - start));
					arguments.push_back(parsedParam.c_str());
					break;
				}
				parsedParam = supplementary::Configuration::trim(currentSettings.substr(start, end - start));
				start = currentSettings.find(",", end);
				if (start != string::npos)
				{
					start += 1;
				}
				arguments.push_back(parsedParam.c_str());
			}
			arguments.push_back(nullptr);
		}
		else
		{
			arguments.push_back(currentSettings.c_str());
			arguments.push_back(nullptr);
		}
		shared_ptr<ChangeSolverSettingsCommand> cmd = make_shared<ChangeSolverSettingsCommand>(arguments, this->mainGui, this, currentSettings);
		this->mainGui->addToCommandHistory(cmd);
		cmd->execute();
		this->close();
	}
}

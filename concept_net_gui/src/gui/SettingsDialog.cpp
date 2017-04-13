/*
 * SettingsDialog.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#include "gui/SettingsDialog.h"
#include "gui/ConceptNetGui.h"
#include "commands/ChangeSolverSettingsCommand.h"
#include "containers/SolverSettings.h"

#include <ui_settingsdialog.h>
#include <ui_conceptnetgui.h>

#include <iostream>
#include <memory>

namespace cng
{
	SettingsDialog::SettingsDialog(QWidget *parent, ConceptNetGui* gui) :
			QDialog(parent), ui(new Ui::SettingsDialog)
	{
		this->mainGui = gui;
		this->ui->setupUi(this);
		connectElements();
//		this->defaultSettings = std::make_shared<SolverSettings>("Default", "clingo, -W, no-atom-undefined, --number=1");
		this->defaultSettings = std::make_shared<SolverSettings>("DefaultClingo5", "clingo");
		this->settingsFile = new QFile(QDir::homePath() + "/.conceptNetSettings.txt");
		this->currentSettings = nullptr;
		loadSettingsFromConfig();
		fillSettingsList();
	}

	Ui::SettingsDialog* SettingsDialog::getUi()
	{
		return this->ui;
	}

	SettingsDialog::~SettingsDialog()
	{
		delete this->settingsFile;
		delete this->ui;
	}


	void SettingsDialog::connectElements()
	{
		this->connect(this->ui->cancelBtn, SIGNAL(released()), this, SLOT(close()));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemClicked(QListWidgetItem * )), this,
						SLOT(fillSettingsLabel(QListWidgetItem * )));
		this->connect(this->ui->okBtn, SIGNAL(released()), this, SLOT(applySettings()));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
						SLOT(fillSettingsLabel(QListWidgetItem *)));
		this->connect(this->ui->settingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
						SLOT(applySettings()));
		this->connect(this->ui->addToSettingsBtn, SIGNAL(released()), this, SLOT(onAddBtn()));
		this->connect(this->ui->removeFromSettingsBtn, SIGNAL(released()), this, SLOT(onRemoveBth()));
		this->connect(this->ui->addToSettingsBtn, SIGNAL(released()), this, SLOT(onAddBtn()));
	}

	void SettingsDialog::loadSettingsFromConfig()
	{
		// default settings
		this->parameterMap.emplace(this->defaultSettings->name, this->defaultSettings);
		//Open settingsfile
		if (!this->settingsFile->open(QIODevice::ReadOnly))
		{
			qWarning("SettingsDialog: Couldn't open file.");
			return;
		}
		// Extract JSON objects from file
		QByteArray loadedData = this->settingsFile->readAll();
		QJsonDocument loadDoc(QJsonDocument::fromJson(loadedData));
		QJsonObject savedObject = loadDoc.object();
		QJsonArray settings = savedObject["settings"].toArray();
		// Parse all saveds settings
		for (int i = 0; i < settings.size(); i++)
		{
			QJsonObject set = settings[i].toObject();
			if (set["name"].toString().toStdString().compare("Default") == 0)
			{
				continue;
			}
			this->parameterMap.emplace(
					set["name"].toString().toStdString(),
					std::make_shared<SolverSettings>(set["name"].toString().toStdString(),
												set["parameters"].toString().toStdString()));

		}
		this->settingsFile->close();
	}

	std::shared_ptr<SolverSettings> SettingsDialog::getDefaultSettings()
	{
		return this->defaultSettings;
	}

	void SettingsDialog::fillSettingsList()
	{
		this->ui->settingsListWidget->clear();
		for (auto pair : this->parameterMap)
		{
			this->ui->settingsListWidget->addItem(QString(pair.first.c_str()));
		}
	}

	void SettingsDialog::fillSettingsLabel(QListWidgetItem* item)
	{
		this->currentSettings = this->parameterMap.at(item->text().toStdString());
		if (this->currentSettings->argString.find(",") != std::string::npos)
		{
			std::string parameters = "";
			for (auto param : this->currentSettings->argumentStrings)
			{
				parameters += param;
				parameters += "\n";
			}
			this->ui->settingsLabel->setText(QString(parameters.c_str()));
		}
		else
		{
			this->ui->settingsLabel->setText(QString(this->currentSettings->argString.c_str()));
		}
	}

	void SettingsDialog::applySettings()
	{

		if (this->currentSettings == nullptr)
		{
			return;
		}
		std::cout << "SettingsDialog: applying parameters: " << currentSettings->argString << std::endl;
		std::shared_ptr<ChangeSolverSettingsCommand> cmd = std::make_shared<ChangeSolverSettingsCommand>(this->mainGui, this,
																								this->currentSettings);
		cmd->execute();
		writeSettings();
		this->close();
	}

	void SettingsDialog::writeSettings()
	{

		if (!this->settingsFile->open(QIODevice::WriteOnly))
		{
			qWarning("SettingsDialog: Couldn't open file.");
			return;
		}

		//JSON Array to hold the commandHistory
		QJsonArray settingsList;
		for (auto set : this->parameterMap)
		{
			QJsonObject obj;
			obj["name"] = QString(set.first.c_str());
			obj["parameters"] = QString(set.second->argString.c_str());
			settingsList.append(obj);
		}
		QJsonObject settings;
		settings["settings"] = settingsList;
		//Write to file
		QJsonDocument settingsDoc(settings);
		this->settingsFile->write(settingsDoc.toJson());
		this->settingsFile->close();
	}

	void SettingsDialog::onAddBtn()
	{
		if (this->ui->paramLineEdit->text().toStdString().empty()
				|| this->ui->nameLineEdit->text().toStdString().empty())
		{
			return;
		}
		this->parameterMap.emplace(
				this->ui->nameLineEdit->text().toStdString(),
				std::make_shared<SolverSettings>(this->ui->nameLineEdit->text().toStdString(),
											this->ui->paramLineEdit->text().toStdString()));
		fillSettingsList();
	}

	void SettingsDialog::onRemoveBth()
	{
		if (this->ui->settingsListWidget->currentItem() != nullptr)
		{
			if (this->ui->settingsListWidget->currentItem()->text().toStdString().compare("Default") == 0)
			{
				return;
			}
			this->parameterMap.erase(this->ui->settingsListWidget->currentItem()->text().toStdString());
		}
		fillSettingsList();
	}

}

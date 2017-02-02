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

#include <iostream>
#include <memory>

namespace cng
{
	SettingsDialog::SettingsDialog(QWidget *parent, ConceptNetGui* gui) :
			QDialog(parent), ui(new Ui::SettingsDialog)
	{
		this->mainGui = gui;
		this->ui->setupUi(this);
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
		this->ui->safeSortedModelscheckBox->setChecked(true);
		this->defaultSettings = make_shared<SolverSettings>("Default", "clingo, -W, no-atom-undefined, --number=1");
		this->settingsFile = new QFile(QDir::homePath() + "/.conceptNetSettings.txt");
		this->currentSettings = nullptr;
		loadSettingsFromConfig();
		fillSettingsList();
	}

	Ui::SettingsDialog* SettingsDialog::getUi()
	{
		return ui;
	}

	SettingsDialog::~SettingsDialog()
	{
		delete this->settingsFile;
		delete ui;
	}

	void SettingsDialog::loadSettingsFromConfig()
	{
		this->parameterMap.emplace(this->defaultSettings->name, this->defaultSettings);
		if (!this->settingsFile->open(QIODevice::ReadOnly))
		{
			qWarning("SettingsDialog: Couldn't open file.");
			return;
		}
		QByteArray loadedData = this->settingsFile->readAll();
		QJsonDocument loadDoc(QJsonDocument::fromJson(loadedData));
		QJsonObject savedObject = loadDoc.object();
		QJsonArray settings = savedObject["settings"].toArray();
		for (int i = 0; i < settings.size(); i++)
		{
			QJsonObject set = settings[i].toObject();
			if (set["name"].toString().toStdString().compare("Default") == 0)
			{
				continue;
			}
			this->parameterMap.emplace(
					set["name"].toString().toStdString(),
					make_shared<SolverSettings>(set["name"].toString().toStdString(),
												set["parameters"].toString().toStdString()));

		}
		this->settingsFile->close();
	}

	shared_ptr<SolverSettings> SettingsDialog::getDefaultSettings()
	{
		return defaultSettings;
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
		if (this->currentSettings->argString.find(",") != string::npos)
		{
			string parameters = "";
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
		cout << "SettingsDialog: applying parameters: " << currentSettings->argString << endl;
		shared_ptr<ChangeSolverSettingsCommand> cmd = make_shared<ChangeSolverSettingsCommand>(this->mainGui, this,
																								currentSettings);
		cmd->execute();
		writeSettings();
		this->close();
	}

	bool SettingsDialog::isSaveSortedChecked()
	{
		return this->ui->safeSortedModelscheckBox->isChecked();
	}

	void SettingsDialog::writeSettings()
	{

		if (!this->settingsFile->open(QIODevice::WriteOnly))
		{
			qWarning("SettingsDialog: Couldn't open file.");
			return;
		}
		QJsonObject settings;

		//JSON Array to hold the commandHistory
		QJsonArray settingsList;
		for (auto set : this->parameterMap)
		{
			QJsonObject obj;
			obj["name"] = QString(set.first.c_str());
			obj["parameters"] = QString(set.second->argString.c_str());
			settingsList.append(obj);
		}
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
				make_shared<SolverSettings>(this->ui->nameLineEdit->text().toStdString(),
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

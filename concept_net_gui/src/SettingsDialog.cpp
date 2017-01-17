/*
 * SettingsDialog.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#include "../include/SettingsDialog.h"
#include <ui_settingsdialog.h>
#include <iostream>
#include <SystemConfig.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
		QDialog(parent), ui(new Ui::SettingsDialog)
{
	this->ui->setupUi(this);
	this->connect(this->ui->cancelBtn, SIGNAL(released()), this, SLOT(close()));
	this->connect(this->ui->commandLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(setCurrentSettings(const QString &)));
	this->connect(this->ui->settingsListWidget, SIGNAL(itemClicked(QListWidgetItem * )), this, SLOT(fillSettingsLabel(QListWidgetItem * )));
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
		this->parameterMap.emplace(section, (*this->sc)[conceptNetGui]->get<string>(conceptNetGui, section.c_str(), "Parameters", NULL));
	}
}

void SettingsDialog::fillSettingsList()
{
	for(auto pair : this->parameterMap)
	{
		this->ui->settingsListWidget->addItem(QString(pair.first.c_str()));
	}
}

void SettingsDialog::fillSettingsLabel(QListWidgetItem* item)
{
	auto params = this->parameterMap.at(item->text().toStdString());
	this->ui->settingsLabel->setText(QString(params.c_str()));
	this->currentSettings = params;
}

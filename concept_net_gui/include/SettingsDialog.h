/*
 * SettingsDialog.h
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_SETTINGSDIALOG_H_
#define INCLUDE_SETTINGSDIALOG_H_

#include <QDialog>
#include <QtGui>
#include <string>
#include <memory>
#include <qlistwidget.h>

using namespace std;

namespace supplementary
{
	class SystemConfig;
}

namespace Ui
{
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
Q_OBJECT

public:
	SettingsDialog(QWidget *parent = 0);
	virtual ~SettingsDialog();
	Ui::SettingsDialog* getUi();
	string getCurrentSettings();

private slots:
	void setCurrentSettings(const QString &text);
	void fillSettingsLabel(QListWidgetItem * item);

private:
	Ui::SettingsDialog* ui;
	string currentSettings;
	supplementary::SystemConfig* sc;
	void loadSettingsFromConfig();
	void fillSettingsList();
	shared_ptr<vector<string>> parameterSectionNames;
	map<string, string> parameterMap;
};

#endif /* INCLUDE_SETTINGSDIALOG_H_ */

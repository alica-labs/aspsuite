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
#include <QListWidget>

#include <string>
#include <memory>

using namespace std;

namespace supplementary
{
	class SystemConfig;
}

namespace Ui
{
	class SettingsDialog;
}
namespace cng
{
	class SolverSettings;
	class ConceptNetGui;
	class SettingsDialog : public QDialog
	{
	Q_OBJECT

	public:
		SettingsDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~SettingsDialog();
		Ui::SettingsDialog* getUi();
		bool isSaveSortedChecked();

	private slots:
		void setCurrentSettings(const QString &text);
		void fillSettingsLabel(QListWidgetItem * item);
		void applySettings();

	private:
		Ui::SettingsDialog* ui;
		ConceptNetGui* mainGui;
		string currentSettings;
		supplementary::SystemConfig* sc;
		void loadSettingsFromConfig();
		void fillSettingsList();
		shared_ptr<vector<string>> parameterSectionNames;
		map<string, shared_ptr<SolverSettings>> parameterMap;
	};
}
#endif /* INCLUDE_SETTINGSDIALOG_H_ */

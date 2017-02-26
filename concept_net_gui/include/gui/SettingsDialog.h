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

namespace Ui
{
	class SettingsDialog;
}
namespace cng
{
	class SolverSettings;
	class ConceptNetGui;
	class AddSettingsDialog;
	class SettingsDialog : public QDialog
	{
	Q_OBJECT

	public:
		SettingsDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~SettingsDialog();
		Ui::SettingsDialog* getUi();
		bool isSaveSortedChecked();
		std::shared_ptr<SolverSettings> getDefaultSettings();

	private slots:
		void fillSettingsLabel(QListWidgetItem * item);
		void applySettings();
		void onAddBtn();
		void onRemoveBth();

	private:
		QFile* settingsFile;
		std::shared_ptr<SolverSettings> defaultSettings;
		Ui::SettingsDialog* ui;
		ConceptNetGui* mainGui;
		std::shared_ptr<SolverSettings> currentSettings;
		void loadSettingsFromConfig();
		void fillSettingsList();
		std::shared_ptr<std::vector<std::string>> parameterSectionNames;
		std::map<std::string, std::shared_ptr<SolverSettings>> parameterMap;
		void writeSettings();
	};
}
#endif /* INCLUDE_SETTINGSDIALOG_H_ */

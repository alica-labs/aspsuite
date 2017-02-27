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
	/**
	 * Handler for setting ui
	 */
	class SettingsDialog : public QDialog
	{
	Q_OBJECT

	public:
		SettingsDialog(QWidget *parent = 0, ConceptNetGui* gui = 0);
		virtual ~SettingsDialog();
		Ui::SettingsDialog* getUi();
		/**
		 * true if save models sorted checkbox is checked
		 */
		bool isSaveSortedChecked();
		/**
		 * Returns shared_ptr to default settings "Default", "clingo, -W, no-atom-undefined, --number=1"
		 */
		std::shared_ptr<SolverSettings> getDefaultSettings();

	private slots:
		/**
		 * Fills settings label with settings given by selected settings item
		 */
		void fillSettingsLabel(QListWidgetItem * item);
		/**
		 * return settings to the main gui
		 */
		void applySettings();
		/**
		 * Ok btn callback
		 */
		void onAddBtn();
		/**
		 * remove btn callback
		 */
		void onRemoveBth();

	private:
		/**
		 * loads settings from json congfig file
		 */
		void loadSettingsFromConfig();
		/**
		 * fill settings table with list items
		 */
		void fillSettingsList();
		/**
		 * save settings to json file
		 */
		void writeSettings();
		QFile* settingsFile;
		std::shared_ptr<SolverSettings> defaultSettings;
		Ui::SettingsDialog* ui;
		ConceptNetGui* mainGui;
		std::shared_ptr<SolverSettings> currentSettings;
		std::shared_ptr<std::vector<std::string>> parameterSectionNames;
		std::map<std::string, std::shared_ptr<SolverSettings>> parameterMap;
	};
}
#endif /* INCLUDE_SETTINGSDIALOG_H_ */

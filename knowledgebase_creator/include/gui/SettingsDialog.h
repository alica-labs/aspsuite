/*
 * SettingsDialog.h
 *
 *  Created on: Jan 17, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_SETTINGSDIALOG_H_
#define INCLUDE_SETTINGSDIALOG_H_

#include <QDialog>
#include <QListWidget>
#include <QtGui>

#include <memory>
#include <string>

namespace Ui
{
class SettingsDialog;
}
namespace kbcr
{
class SolverSettings;
class KnowledgebaseCreator;
/**
 * Handler for setting ui
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    SettingsDialog(QWidget* parent = 0, KnowledgebaseCreator* gui = 0);
    /**
     * Destructor
     */
    virtual ~SettingsDialog();
    /**
     * Get Pointer to Settings Dialog
     */
    Ui::SettingsDialog* getUi();
    /**
     * Returns shared_ptr to default settings "Default", "clingo, -W, no-atom-undefined, --number=1"
     */
    std::shared_ptr<SolverSettings> getDefaultSettings();

private slots:
    /**
     * Fills settings label with settings given by selected settings item
     */
    void fillSettingsLabel(QListWidgetItem* item);
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
    /**
     * Connect every gui element
     */
    void connectElements();
    /**
     * Pointer to file containing solver settings
     */
    QFile* settingsFile;
    /**
     * Default settings "Default", "clingo, -W, no-atom-undefined, --number=1"
     */
    std::shared_ptr<SolverSettings> defaultSettings;
    /**
     * Pointer to Settings Dialog
     */
    Ui::SettingsDialog* ui;
    /**
     * Pointer to main gui
     */
    KnowledgebaseCreator* mainGui;
    /**
     * Current settings pointer
     */
    std::shared_ptr<SolverSettings> currentSettings;
    /**
     * List of Parameter names show in dialog
     */
    std::shared_ptr<std::vector<std::string>> parameterSectionNames;
    /**
     * Mapping of names to settings
     */
    std::map<std::string, std::shared_ptr<SolverSettings>> parameterMap;
};
} // namespace kbcr
#endif /* INCLUDE_SETTINGSDIALOG_H_ */

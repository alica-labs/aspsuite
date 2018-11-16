#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>
#include <vector>

namespace kbcr
{

class SolverSettings;
class KnowledgebaseCreator;
class SettingsDialog;
/**
 * Class inheriting from Command interface used to Change Solver Settings
 */
class ChangeSolverSettingsCommand : public Command, public std::enable_shared_from_this<ChangeSolverSettingsCommand>
{
    Q_OBJECT
public:
    ChangeSolverSettingsCommand(KnowledgebaseCreator* gui, SettingsDialog* dialog, std::shared_ptr<SolverSettings> settings);
    virtual ~ChangeSolverSettingsCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    std::shared_ptr<SolverSettings> previousSettings;
    std::shared_ptr<SolverSettings> currentSettings;
    KnowledgebaseCreator* gui;
    SettingsDialog* dialog;
};

} /* namespace kbcr */


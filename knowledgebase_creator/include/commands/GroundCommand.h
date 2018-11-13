#pragma once

#include <QJsonObject>
#include <QString>

#include <commands/Command.h>
#include <memory>

namespace kbcr
{
class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to call the solvers Ground method
 */
class GroundCommand : public Command, public std::enable_shared_from_this<GroundCommand>
{
    Q_OBJECT
public:
    GroundCommand(KnowledgebaseCreator* gui, QString program);
    virtual ~GroundCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString program;
    QString programSection;
    std::string historyProgramSection;

private:
    /**
     * Extract program section from input
     */
    void extractProgramSection();
};

} /* namespace kbcr */


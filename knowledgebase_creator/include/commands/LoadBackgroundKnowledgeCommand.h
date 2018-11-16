#pragma once

#include "commands/Command.h"

#include <QByteArray>
#include <QJsonObject>
#include <QString>

#include <memory>

namespace kbcr
{
class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to load a BackgroundKnowledge file
 */
class LoadBackgroundKnowledgeCommand : public Command, public std::enable_shared_from_this<LoadBackgroundKnowledgeCommand>
{
    Q_OBJECT
public:
    LoadBackgroundKnowledgeCommand(KnowledgebaseCreator* gui, QString fileName);
    virtual ~LoadBackgroundKnowledgeCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QByteArray fileContent;
    QString fileName;
};

} /* namespace kbcr */


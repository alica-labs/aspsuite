#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>
#include <string>

namespace kbcr
{

class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to create a ExtensionQuery
 */
class ExtensionQueryCommand : public Command, public std::enable_shared_from_this<ExtensionQueryCommand>
{
    Q_OBJECT
public:
    ExtensionQueryCommand(KnowledgebaseCreator* gui, QString program);
    virtual ~ExtensionQueryCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString program;
    QString toShow;
};

} /* namespace kbcr */


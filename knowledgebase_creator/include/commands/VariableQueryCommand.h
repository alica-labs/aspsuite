#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>
#include <string>

namespace kbcr
{

class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to create a VariableQuery
 */
class VariableQueryCommand : public Command, public std::enable_shared_from_this<VariableQueryCommand>
{
    Q_OBJECT
public:
    VariableQueryCommand(KnowledgebaseCreator* gui, QString program);
    virtual ~VariableQueryCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString program;
    QString toShow;
};

} /* namespace kbcr */


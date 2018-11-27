#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>

namespace kbcr
{
class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to create a FilterQuery
 */
class FilterQueryCommand : public Command, public std::enable_shared_from_this<FilterQueryCommand>
{
    Q_OBJECT
public:
    FilterQueryCommand(KnowledgebaseCreator* gui, QString factsString);
    virtual ~FilterQueryCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString factsString;
};

} /* namespace kbcr */


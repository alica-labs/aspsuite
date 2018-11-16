#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>

namespace kbcr
{
class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to create a FactsQuery
 */
class FactsQueryCommand : public Command, public std::enable_shared_from_this<FactsQueryCommand>
{
    Q_OBJECT
public:
    FactsQueryCommand(KnowledgebaseCreator* gui, QString factsString);
    virtual ~FactsQueryCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString factsString;
};

} /* namespace kbcr */


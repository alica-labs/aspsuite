#pragma once

#include "commands/Command.h"

#include <QJsonObject>

#include <memory>

namespace kbcr
{

class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used to call the solvers Add method
 */
class AddCommand : public Command, public std::enable_shared_from_this<AddCommand>
{
    Q_OBJECT
public:
    AddCommand(KnowledgebaseCreator* gui, QString program);
    virtual ~AddCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString program;
    QString programSection;
    std::string historyProgramSection;

private:
    void extractProgramSection();
};

} /* namespace kbcr */

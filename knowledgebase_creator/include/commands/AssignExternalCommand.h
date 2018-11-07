/*
 * AssignExternalCommand.h
 *
 *  Created on: Mar 11, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_
#define INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_

#include <QJsonObject>
#include <commands/Command.h>
#include <memory>

namespace kbcr
{

class KnowledgebaseCreator;
/**
 * Class inheriting from Command interface used assign truth values to an external
 */
class AssignExternalCommand : public Command, public std::enable_shared_from_this<AssignExternalCommand>
{
public:
    AssignExternalCommand(KnowledgebaseCreator* gui, QString externalName, QString truthValue);
    virtual ~AssignExternalCommand();

    void execute();
    void undo();

    QJsonObject toJSON();

    KnowledgebaseCreator* gui;
    QString externalName;
    QString truthValue;
    QString historyString;
    bool previousTruthValue;

private:
    void createHistoryString();
};

} /* namespace kbcr */

#endif /* INCLUDE_COMMANDS_ASSIGNEXTERNALCOMMAND_H_ */

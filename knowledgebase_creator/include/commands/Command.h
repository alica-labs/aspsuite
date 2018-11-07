/*
 * Command.h
 *
 *  Created on: Jan 18, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMAND_H_
#define INCLUDE_COMMAND_H_

#include <QJsonObject>
#include <QObject>
#include <string>

namespace kbcr
{
/**
 * Command interface
 */
class Command : public QObject
{
    Q_OBJECT
public:
    virtual ~Command(){};
    /**
     * Execute the command
     */
    virtual void execute() = 0;
    /**
     * Undo the Command
     */
    virtual void undo() = 0;

    /**
     * Creates JSON String to be saved
     * @return QJsonObject
     */
    virtual QJsonObject toJSON() = 0;

    /**
     * Gets type of Command
     * @return type string
     */
    std::string getType() { return type; };

protected:
    std::string type = "interface";
};
} // namespace kbcr

#endif /* INCLUDE_COMMAND_H_ */

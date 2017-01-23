/*
 * Command.h
 *
 *  Created on: Jan 18, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMAND_H_
#define INCLUDE_COMMAND_H_

#include <string>
#include <QJsonObject>

using namespace std;

namespace cng
{
	/**
	 * Command interface
	 */
	class Command
	{
	public:
		virtual ~Command() {};
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
		string getType() {return type;};

	protected:
		string type = "interface";

	};
}

#endif /* INCLUDE_COMMAND_H_ */

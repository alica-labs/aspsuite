/*
 * Command.h
 *
 *  Created on: Jan 18, 2017
 *      Author: stefan
 */

#ifndef INCLUDE_COMMAND_H_
#define INCLUDE_COMMAND_H_

#include <string>

using namespace std;

namespace cng
{
	class Command
	{
	public:
		virtual ~Command() {};

		virtual void execute() = 0;
		virtual void undo() = 0;

		string getType() {return type;};

	protected:
		string type = "interface";

	};
}

#endif /* INCLUDE_COMMAND_H_ */

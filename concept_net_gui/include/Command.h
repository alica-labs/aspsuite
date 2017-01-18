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

class Command
{
public:
	Command();
	virtual ~Command();

	string type;
	long id;
	string aspProgram;

};

#endif /* INCLUDE_COMMAND_H_ */

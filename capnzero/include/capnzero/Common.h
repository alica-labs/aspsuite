#pragma once

#include <zmq.h>

#include <string>
#include <iostream>


namespace capnzero{

	/**
	 * Checks the return code and reports an error if present.
	 * If abortIfError is set to true, it also aborts the process.
	 */
	inline void check(int returnCode, std::string methodName)
	{
		if (returnCode != 0)
		{
			std::cerr << methodName << " returned: " << errno << " - " << zmq_strerror(errno) << std::endl;
		}
	}
}

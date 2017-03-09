#include "discovery/Test.h"

#include <zmq.h>
#include <iostream>

namespace discovery
{

Test::Test()
{
}

Test::~Test()
{
}

}

int main (int argc, char** argv)
{
	int major, minor, patch;
	zmq_version (&major, &minor, &patch);
	std::cout << "Major: " << major << " Minor: " << minor << " Patch: " << patch << std::endl;
}

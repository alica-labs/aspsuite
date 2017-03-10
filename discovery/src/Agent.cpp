#include "discovery/Agent.h"

#include <zmq.h>

#include <chrono>
#include <iostream>
#include <unistd.h>

namespace discovery
{

bool Agent::operating = true;

Agent::Agent() : Worker("Bob")
{
    // generate
    uuid_generate(this->uuid);
}

Agent::~Agent()
{
}

void Agent::run()
{
	this->checkZMQVersion();
	this->testUUIDStuff();
}

void Agent::testUUIDStuff()
{
	uuid_generate(this->uuid);

    // unparse (to string)
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_lower(uuid, uuid_str);
    printf("generate uuid=%s\n", uuid_str);

    // parse (from string)
    uuid_t uuid2;
    uuid_parse(uuid_str, uuid2);

    // compare (rv == 0)
    int rv;
    rv = uuid_compare(uuid, uuid2);
    printf("uuid_compare() result=%d\n", rv);

    // compare (rv == 1)
    uuid_t uuid3;
    uuid_parse("1b4e28ba-2fa1-11d2-883f-0016d3cca427", uuid3);
    rv = uuid_compare(uuid, uuid3);
    printf("uuid_compare() result=%d\n", rv);

    // is null? (rv == 0)
    rv = uuid_is_null(uuid);
    printf("uuid_null() result=%d\n", rv);

    // is null? (rv == 1)
    uuid_clear(uuid);
    rv = uuid_is_null(uuid);
    printf("uuid_null() result=%d\n", rv);
}

void Agent::checkZMQVersion()
{
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	std::cout << "Major: " << major << " Minor: " << minor << " Patch: " << patch << std::endl;
}

void Agent::sigIntHandler(int sig)
{
	operating = false;
}
}

int main(int argc, char **argv)
{
	discovery::Agent* bob = new discovery::Agent();

	bob->setDelayedStartMS(std::chrono::milliseconds(0));
	bob->setIntervalMS(std::chrono::milliseconds(1000));

	bob->start();

    // register ctrl+c handler
    signal(SIGINT, discovery::Agent::sigIntHandler);

    // start running
    while(discovery::Agent::operating)
    {
    	sleep(1);
    }

    bob->stop();

	delete bob;
    return 0;
}

#pragma once

/* Next 2 Lines: To get defns of NI_MAXSERV and NI_MAXHOST */
//#define _GNU_SOURCE
#include <netdb.h>

#include "discovery/Worker.h"

#include <zmq.h>
#include <uuid/uuid.h>

#include <kj/array.h>
#include <capnp/common.h>

#include <signal.h>
#include <vector>


namespace discovery
{

class Agent : public Worker
{
  public:
    Agent(std::string name, bool sender);
    virtual ~Agent();

    void run();

    static void sigIntHandler(int sig);
    static bool operating;

  private:
    void setupReceiveUDPMulticast();
    void setupSendUDPMulticast();

    void sendMultiPartZeroCopy();
    void send();

    void receive();
    kj::ArrayPtr<kj::ArrayPtr<capnp::word const>> genericReceive ();

    void testUUIDStuff();
    void checkZMQVersion();
    bool getWirelessAddress();
    void check(int returnCode, std::string methodName, bool abortIfError = false);

    uuid_t uuid;
    char wirelessIpAddress[NI_MAXHOST];

    // zmq stuff
    bool sender;
    void *ctx;
    void *socket;
};

} /* namespace discovery */

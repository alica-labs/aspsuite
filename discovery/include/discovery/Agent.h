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

    uuid_t uuid;
    char wirelessIpAddress[NI_MAXHOST];

    //std::vector<std::string> parts_;
    //std::vector<std::unique_ptr<capnp::word[]>> copied_parts_;
    std::vector<kj::ArrayPtr<capnp::word const>> segments_;

    // zmq stuff
    bool sender;
    void *ctx;
    void *socket;
};

} /* namespace discovery */

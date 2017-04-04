#pragma once

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
    void send();
    int msg_send (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_);
    void receive();
    kj::ArrayPtr<kj::ArrayPtr<capnp::word const>> genericReceive ();
    int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_);
    void testUUIDStuff();
    void checkZMQVersion();
    bool getWirelessAddress();
    uuid_t uuid;
    char* wirelessIpAddress;


    std::vector<std::string> parts_;
    std::vector<std::unique_ptr<capnp::word[]>> copied_parts_;
    std::vector<kj::ArrayPtr<capnp::word const>> segments_;




    // zmq stuff
    bool sender;
    void *ctx;
    void *socket;
};

} /* namespace discovery */

#pragma once

#include "discovery/Worker.h"

#include <zmq.h>
#include <uuid/uuid.h>
#include <signal.h>


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
    int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_);
    void testUUIDStuff();
    void checkZMQVersion();
    uuid_t uuid;

    // zmq stuff
    bool sender;
    void *ctx;
    void *socket;
};

} /* namespace discovery */

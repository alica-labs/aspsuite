#pragma once

#include "discovery/Worker.h"

#include <uuid/uuid.h>
#include <signal.h>

namespace discovery
{

class Agent : public Worker
{
  public:
    Agent();
    virtual ~Agent();

    void run();

    static void sigIntHandler(int sig);
    static bool operating;

  private:
    void testUUIDStuff();
    void checkZMQVersion();
    uuid_t uuid;
};

} /* namespace discovery */

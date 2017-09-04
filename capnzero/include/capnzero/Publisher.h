#pragma once

#include "Common.h"
#include <capnp/serialize-packed.h>
#include <zmq.h>

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>

namespace capnzero
{

class Publisher
{

  public:
    Publisher(void *context, std::string connection, std::string multicastGroupName);
    virtual ~Publisher();


    int send(std::shared_ptr<::capnp::MallocMessageBuilder> msgBuilder);

    std::string multicastGroupName;

  protected:
    void *context;
    void *socket;

    std::vector<std::shared_ptr<::capnp::MallocMessageBuilder>> sentMsgsWaitingForCleanup;
};

static void cleanUpMsgData(void * data, void * hint);
}

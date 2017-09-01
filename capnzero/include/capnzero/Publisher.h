#pragma once

#include "Common.h"
#include <capnp/serialize-packed.h>
#include <zmq.h>

#include <iostream>
#include <string>

namespace capnzero
{

class Publisher
{

  public:
    Publisher(void *context);
    virtual ~Publisher(){};

    template <typename CapnProtoMsgBuilder>
    int send(CapnProtoMsgBuilder &msg);

    std::string multicastGroupName;

  protected:
    void *context;
	void *socket;
};
}

template <typename CapnProtoMsgBuilder>
inline int capnzero::Publisher::send(CapnProtoMsgBuilder &msgBuilder)
{
    zmq_msg_t msg;
    auto wordArray = capnp::messageToFlatArray(msgBuilder);

    check(zmq_msg_init_data(&msg, wordArray->begin(), wordArray->size() * sizeof(capnp::word), NULL, NULL),
          "zmq_msg_init_data");

    /* uncomment for printing bytes */
    //    auto msgByteArray = reinterpret_cast<char *>(zmq_msg_data(&msg));
    //    for (int i = 0; i < zmq_msg_size(&msg); i++)
    //    {
    //    	std::cout << std::hex << msgByteArray[i] << ":";
    //
    //    }
    //    std::cout << std::endl;

    // set group
    check(zmq_msg_set_group(&msg, this->multicastGroupName.c_str()), "zmq_msg_set_group");

    // send
    int numBytesSend = zmq_msg_send(&msg, this->socket, 0);
    if (numBytesSend == -1)
    {
        std::cerr << "zmq_msg_send was unsuccessfull: " << errno << " - " << zmq_strerror(errno) << std::endl;
        check(zmq_msg_close(&msg), "zmq_msg_close");
    }

    return numBytesSend;
}

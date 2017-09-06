#include "capnzero/Publisher.h"

#include <zmq.h>
#include <unistd.h>

capnzero::Publisher::Publisher(void *context, std::string connection, std::string multicastGroupName)
    : socket(nullptr)
    , context(context)
    , multicastGroupName(multicastGroupName)
{
    this->socket = zmq_socket(this->context, ZMQ_RADIO);
    check(zmq_connect(socket, connection.c_str()), "zmq_connect");
}

capnzero::Publisher::~Publisher()
{
    check(zmq_close(this->socket), "zmq_close");
}

static void capnzero::cleanUpMsgData(void *data, void *hint)
{
	std::cout << "Cleanup" << std::endl;
    delete reinterpret_cast<kj::Array<capnp::word>*>(hint);
}

int capnzero::Publisher::send(kj::Array<capnp::word> * wordArrayPtr)
{
    // setup zmq msg
    zmq_msg_t msg;

    check(zmq_msg_init_data(&msg, wordArrayPtr->begin(), wordArrayPtr->size() * sizeof(capnp::word), &cleanUpMsgData, wordArrayPtr),
          "zmq_msg_init_data");

    // set group
    check(zmq_msg_set_group(&msg, this->multicastGroupName.c_str()), "zmq_msg_set_group");

    // send
    int numBytesSend = zmq_msg_send(&msg, this->socket, 0);
    if (numBytesSend == -1)
    {
        std::cerr << "zmq_msg_send was unsuccessfull: Errno " << errno << " means: " << zmq_strerror(errno)
                  << std::endl;
        check(zmq_msg_close(&msg), "zmq_msg_close");
    }

    return numBytesSend;
}

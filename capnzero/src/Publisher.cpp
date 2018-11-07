#include "capnzero/Publisher.h"

#include <unistd.h>
#include <zmq.h>

namespace capnzero
{

Publisher::Publisher(void* context, std::string connection, std::string multicastGroupName)
        : socket(nullptr)
        , multicastGroupName(multicastGroupName)
{
    this->socket = zmq_socket(context, ZMQ_RADIO);
    check(zmq_connect(socket, connection.c_str()), "zmq_connect");
}

Publisher::~Publisher()
{
    check(zmq_close(this->socket), "zmq_close");
}

static void cleanUpMsgData(void* data, void* hint)
{
    delete reinterpret_cast<kj::Array<capnp::word>*>(hint);
}

int Publisher::send(::capnp::MallocMessageBuilder& msgBuilder)
{
    // setup zmq msg
    zmq_msg_t msg;

    kj::Array<capnp::word> wordArray = capnp::messageToFlatArray(msgBuilder);
    kj::Array<capnp::word>* wordArrayPtr = new kj::Array<capnp::word>(kj::mv(wordArray)); // will be delete by zero-mq
    check(zmq_msg_init_data(&msg, wordArrayPtr->begin(), wordArrayPtr->size() * sizeof(capnp::word), &cleanUpMsgData, wordArrayPtr), "zmq_msg_init_data");

    // set group
    check(zmq_msg_set_group(&msg, this->multicastGroupName.c_str()), "zmq_msg_set_group");

    // send
    int numBytesSend = zmq_msg_send(&msg, this->socket, 0);
    if (numBytesSend == -1) {
        std::cerr << "zmq_msg_send was unsuccessful: Errno " << errno << " means: " << zmq_strerror(errno) << std::endl;
        check(zmq_msg_close(&msg), "zmq_msg_close");
    }

    return numBytesSend;
}
} // namespace capnzero

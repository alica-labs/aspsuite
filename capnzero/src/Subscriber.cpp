#include "capnzero/Subscriber.h"



#include <assert.h>

namespace capnzero
{

const int Subscriber::wordSize = sizeof(capnp::word);

Subscriber::~Subscriber()
{
    check(zmq_close(this->socket), "zmq_close");
}

void Subscriber::receive()
{
    zmq_msg_t msg;
    check(zmq_msg_init(&msg), "zmq_msg_init");
    int nbytes = zmq_msg_recv(&msg, this->socket, 0);

    // Received message must contain an integral number of words.
    if (zmq_msg_size(&msg) % Subscriber::wordSize == 0)
    {
        check(zmq_msg_close(&msg), "zmq_msg_close");
        return;
    }

    // Check whether message is memory aligned
    assert(reinterpret_cast<uintptr_t>(zmq_msg_data(&msg)) % Subscriber::wordSize == 0);

    int numWordsInMsg = zmq_msg_size(&msg);
    auto wordArray =
        kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(zmq_msg_data(&msg)), numWordsInMsg);

    ::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(wordArray);

//    auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();
//
//    std::cout << "Subscriber::receive(): " << beacon.toString().flatten().cStr() << std::endl;

    check(zmq_msg_close(&msg), "zmq_msg_close");
}
}

#pragma once

#include "capnzero/Common.h"

#include <capnp/serialize-packed.h>

#include <zmq.h>

#include <string>

#include <assert.h>

namespace capnzero
{

template <class ObjType>
class Subscriber
{
  public:
	typedef void (ObjType::*callbackFunction)(::capnp::FlatArrayMessageReader&);

    template <class CallbackObjType>
    Subscriber(void *context, std::string connection, std::string multicastGroupName,
               void (CallbackObjType::*callbackFunction)(::capnp::FlatArrayMessageReader&), CallbackObjType *callbackObject)
        : socket(nullptr)
        , multicastGroupName(multicastGroupName)
        , callbackObject_(callbackObject)
        , callbackFunction_(callbackFunction)
    {
        this->socket = zmq_socket(context, ZMQ_DISH);
        check(zmq_bind(this->socket, connection.c_str()), "zmq_connect");
        check(zmq_join(this->socket, this->multicastGroupName.c_str()), "zmq_join");
    }
    virtual ~Subscriber();
    void receive();

    static const int wordSize;

    ObjType *callbackObject_;
    //void (ObjType::*callbackFunction_)(MsgType::Reader);
    callbackFunction callbackFunction_;

  protected:
    void *socket;
    std::string multicastGroupName;
};

template <typename ObjType>
const int Subscriber<ObjType>::wordSize = sizeof(capnp::word);

template <typename ObjType>
Subscriber<ObjType>::~Subscriber()
{
    check(zmq_close(this->socket), "zmq_close");
}

template <typename ObjType>
void Subscriber<ObjType>::receive()
{
    zmq_msg_t msg;
    check(zmq_msg_init(&msg), "zmq_msg_init");
    std::cout << "Subscriber<ObjType>::receive() called!" << std::endl;
    int nbytes = zmq_msg_recv(&msg, this->socket, 0);

    // Received message must contain an integral number of words.
    if (zmq_msg_size(&msg) % Subscriber::wordSize != 0)
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

    (this->callbackObject_->*callbackFunction_)(msgReader);

    //    auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();
    //
    //    std::cout << "Subscriber::receive(): " << beacon.toString().flatten().cStr() << std::endl;

    check(zmq_msg_close(&msg), "zmq_msg_close");
}
}

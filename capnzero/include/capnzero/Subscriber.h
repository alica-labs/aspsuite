#pragma once

#include "capnzero/Common.h"

#include <capnp/serialize-packed.h>

#include <zmq.h>

#include <string>

#include <assert.h>
#include <functional>

namespace capnzero
{

class Subscriber
{
  public:
    typedef std::function<void(::capnp::FlatArrayMessageReader &)> callbackFunction;
    // typedef void (ObjType::*callbackMemberFunction)(::capnp::FlatArrayMessageReader &);
    // typedef void (*callbackFunction)(::capnp::FlatArrayMessageReader &);

    //    template <class CallbackObjType>
    //    Subscriber(void *context, std::string connection, std::string multicastGroupName,
    //               void (CallbackObjType::*callbackFunction)(::capnp::FlatArrayMessageReader &),
    //               CallbackObjType *callbackObject)
    //        : socket(nullptr)
    //        , multicastGroupName(multicastGroupName)
    //        , callbackObject_(callbackObject)
    //        , callbackFunction_(callbackFunction)
    //		, callbackMemberFunction_(nullptr)
    //    {
    //        this->socket = zmq_socket(context, ZMQ_DISH);
    //        check(zmq_bind(this->socket, connection.c_str()), "zmq_connect");
    //        check(zmq_join(this->socket, this->multicastGroupName.c_str()), "zmq_join");
    //    }

    Subscriber(void *context, std::string connection, std::string multicastGroupName)
        : socket(nullptr)
        , multicastGroupName(multicastGroupName)
        , callbackFunction_(nullptr)
    {
        this->socket = zmq_socket(context, ZMQ_DISH);
        check(zmq_bind(this->socket, connection.c_str()), "zmq_connect");
        check(zmq_join(this->socket, this->multicastGroupName.c_str()), "zmq_join");
    }

    virtual ~Subscriber();

    template <class CallbackObjType>
    void subscribe(void (CallbackObjType::*callbackFunction)(::capnp::FlatArrayMessageReader &),
                   CallbackObjType *callbackObject);

    void subscribe(void (*callbackFunction)(::capnp::FlatArrayMessageReader &));

    void receive();

    static const int wordSize;

    // ObjType *callbackObject_;
    // void (ObjType::*callbackFunction_)(MsgType::Reader);
    // callbackMemberFunction callbackMemberFunction_;
    callbackFunction callbackFunction_;

  protected:
    void *socket;
    std::string multicastGroupName;
};

const int Subscriber::wordSize = sizeof(capnp::word);

Subscriber::~Subscriber()
{
    check(zmq_close(this->socket), "zmq_close");
}

template <class CallbackObjType>
void Subscriber::subscribe(void (CallbackObjType::*callbackFunction)(::capnp::FlatArrayMessageReader &),
                           CallbackObjType *callbackObject)
{
	using std::placeholders::_1;
	this->callbackFunction_ = std::bind(callbackFunction, callbackObject, _1);
}

void Subscriber::subscribe(void (*callbackFunction)(::capnp::FlatArrayMessageReader &))
{
	this->callbackFunction_ = callbackFunction;
}

void Subscriber::receive()
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

    (this->callbackFunction_)(msgReader);

    //    auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();
    //
    //    std::cout << "Subscriber::receive(): " << beacon.toString().flatten().cStr() << std::endl;

    check(zmq_msg_close(&msg), "zmq_msg_close");
}
}

#pragma once

#include "capnzero/Common.h"

#include <capnp/serialize-packed.h>

#include <zmq.h>

#include <string>

namespace capnzero
{

template <class MsgType, class ObjType>
class Subscriber
{
  public:
	template <class CapnProtoMsgType, class CallbackObjType>
    Subscriber(void *context, std::string connection, std::string multicastGroupName,
               void (CallbackObjType::*fp)(CapnProtoMsgType), CallbackObjType *obj)
	    : socket(nullptr)
	    , multicastGroupName(multicastGroupName)
		, obj(obj)
	{
	    this->socket = zmq_socket(context, ZMQ_DISH);
	    check(zmq_connect(this->socket, connection.c_str()), "zmq_connect");
	}
    virtual ~Subscriber();
    void receive();

    static const int wordSize;

    ObjType* obj;


  protected:
    void *socket;
    std::string multicastGroupName;
};
}

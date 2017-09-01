#include <capnzero/Publisher.h>
#include <zmq.h>


capnzero::Publisher::Publisher(void * context)
    : socket(nullptr), context(context)
{
	this->socket = zmq_socket(this->context, ZMQ_RADIO);
	check(zmq_connect(socket, "udp://224.0.0.1:5555"), "zmq_connect");
}

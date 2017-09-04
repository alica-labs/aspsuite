#include "capnzero/Publisher.h"

#include <zmq.h>

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
    ((std::shared_ptr<::capnp::MallocMessageBuilder>*) hint)->reset();

}

int capnzero::Publisher::send(std::shared_ptr<::capnp::MallocMessageBuilder> msgBuilder)
{
	// clean up deleted msgs
    for (int i = 0; i < this->sentMsgsWaitingForCleanup.size(); i++)
    {
    	if (this->sentMsgsWaitingForCleanup[i])  // test whether it is empty
    	{
    		this->sentMsgsWaitingForCleanup.erase(this->sentMsgsWaitingForCleanup.begin()+i);
    	}
    }

    // remember new message
    this->sentMsgsWaitingForCleanup.push_back(msgBuilder);

    // setup zmq msg
    zmq_msg_t msg;
    auto wordArray = capnp::messageToFlatArray(*msgBuilder);
    check(zmq_msg_init_data(&msg, wordArray.begin(), wordArray.size() * sizeof(capnp::word), cleanUpMsgData,
                            (void *) &this->sentMsgsWaitingForCleanup.back()),
          "zmq_msg_init_data");

    /* uncomment for printing bytes */
    //    auto msgByteArray = reinterpret_cast<char *>(zmq_msg_data(&msg));
    //    for (int i = 0; i < zmq_msg_size(&msg); i++)
    //    {
    //        std::cout << std::hex << std::setfill('0') << std::setw(2) << msgByteArray[i] << ":";
    //    }
    //    std::cout << std::endl;

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

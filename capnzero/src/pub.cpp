#include "capnzero-base-msgs/string.capnp.h"

#include <capnzero/Publisher.h>

#include <capnp/common.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <kj/array.h>

#define DEBUG_SENDER

int main(int argc, char** argv)
{

    if (argc <= 1) {
        std::cerr << "Synopsis: rosrun capnzero pub \"String that should be published!\"" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    // Cap'n Proto: create proto message

    // init builder
    ::capnp::MallocMessageBuilder msgBuilder;
    capnzero::String::Builder beaconMsgBuilder = msgBuilder.initRoot<capnzero::String>();

    // set content
    beaconMsgBuilder.setString(argv[2]);

#ifdef DEBUG_SENDER
    std::cout << "pub: Message to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;
#endif

    // void* context, std::string connection, std::string multicastGroupName
    void* ctx = zmq_ctx_new();
    capnzero::Publisher pub = capnzero::Publisher(ctx, "udp://224.0.0.1:5555", argv[1]);
    int numBytesSent = pub.send(msgBuilder);

#ifdef DEBUG_SENDER
    std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
}

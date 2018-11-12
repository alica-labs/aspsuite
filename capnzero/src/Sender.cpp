#include <capnzero/Publisher.h>

//#include <capnzero/string.capnp.h>

#include <capnp/common.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <kj/array.h>

int main (int argc, char ** argv) {

    if (argc <= 1)
    {
        std::cerr << "Synopsis: rosrun capnzero pub \"String that should be published!\"" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    void* ctx = zmq_ctx_new();

    // Cap'n Proto: create proto message

    // init builder
    ::capnp::MallocMessageBuilder msgBuilder;
//    capnp::String::Builder beaconMsgBuilder = msgBuilder.initRoot<capnp::Text>();

    // set content
//    beaconMsgBuilder.

#ifdef DEBUG_AGENT
    std::cout << "pub: Message to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;
#endif

    //void* context, std::string connection, std::string multicastGroupName
//    capnzero::Publisher pub = capnzero::Publisher(ctx);
//    int numBytesSent = pub->send(msgBuilder);

#ifdef DEBUG_AGENT
    std::cout << "pub: " << numBytesSent << " Bytes sent!" << std::endl;
#endif
}

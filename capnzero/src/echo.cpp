#include "capnzero-base-msgs/string.capnp.h"

#include <capnzero/Subscriber.h>

#include <capnp/common.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <kj/array.h>

#include <signal.h>
#include <thread>

#define DEBUG_SENDER

static bool running;

void callback(::capnp::FlatArrayMessageReader& reader)
{
    std::cout << "Called callback..." << std::endl;
    std::cout << reader.getRoot<capnzero::String>().toString().flatten().cStr() << std::endl;
}

void sigIntHandler(int sig)
{
    running = false;
}

int main(int argc, char** argv)
{
    running = true;

    if (argc <= 1) {
        std::cerr << "Synopsis: rosrun capnzero echo \"Topic that should be listened to!\"" << std::endl;
        return -1;
    }

    for (size_t i = 0; i < argc; i++) {
        std::cout << "Param " << i << ": '" << argv[i] << "'" << std::endl;
    }

    signal(SIGINT, sigIntHandler);

    void* ctx = zmq_ctx_new();
    capnzero::Subscriber sub = capnzero::Subscriber(ctx, "udp://224.0.0.1:5555", argv[1]);
    sub.subscribe(&callback);

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}

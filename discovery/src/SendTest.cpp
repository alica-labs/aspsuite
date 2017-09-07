#include <capnzero/Publisher.h>

#include <discovery_msgs/beacon.capnp.h>
#include <zmq.h>

#include <uuid/uuid.h>

#include <assert.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <iostream>
#include <unistd.h>
#include <memory>

/**
 * Checks the return code and reports an error if present.
 * If abortIfError is set to true, it also aborts the process.
 */
void check(int returnCode, std::string methodName, bool abortIfError)
{
    if (returnCode != 0)
    {
        std::cerr << methodName << " returned: " << errno << " - " << zmq_strerror(errno) << std::endl;
        if (abortIfError)
            assert(returnCode);
    }
}

void send(capnzero::Publisher* pub)
{
    capnp::MallocMessageBuilder msgBuilder;
    discovery_msgs::Beacon::Builder beaconMsgBuilder = msgBuilder.initRoot<discovery_msgs::Beacon>();

    // set content
    beaconMsgBuilder.setIp("192.186.0.1");
    beaconMsgBuilder.setPort(6666);
    uuid_t uuid;
    uuid_generate(uuid);
    beaconMsgBuilder.setUuid(kj::arrayPtr(uuid, sizeof(uuid)));

    //std::cout << "SendTest::send(): Message to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;

    int numBytesSent = pub->send(msgBuilder);

    std::cout << "SendTest::send(): " << numBytesSent << " Bytes sent! " << std::endl;
}

int main(int argc, char **argv)
{
    auto ctx = zmq_ctx_new();
    assert(ctx);

    capnzero::Publisher* pub = new capnzero::Publisher(ctx, "udp://224.0.0.1:5555", "MCGroup");

    send(pub);

    delete(pub);
    check(zmq_ctx_term(ctx), "zmq_ctx_term", true);
}

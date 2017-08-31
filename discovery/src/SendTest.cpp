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

void free (void * data, void * hint)
{
}

void send(void * socket)
{
    ::capnp::MallocMessageBuilder msgBuilder;
    discovery_msgs::Beacon::Builder beaconMsgBuilder = msgBuilder.initRoot<discovery_msgs::Beacon>();

    // set content
    beaconMsgBuilder.setIp("192.186.0.1");
    beaconMsgBuilder.setPort(6666);
    uuid_t uuid;
    uuid_generate(uuid);
    beaconMsgBuilder.setUuid(kj::arrayPtr(uuid, sizeof(uuid)));

    zmq_msg_t msg;
    auto wordArray = std::shared_ptr<kj::Array<capnp::word>>(new kj::Array<capnp::word>(capnp::messageToFlatArray(msgBuilder)));

    std::cout << "Send Message: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;

    check(zmq_msg_init_data(&msg, wordArray->begin(), wordArray->size() * sizeof(capnp::word), std::get_deleter<kj::Array<capnp::word>>(wordArray), NULL),
          "zmq_msg_init_data", false);

    /* uncomment for printing bytes */
    //    auto msgByteArray = reinterpret_cast<char *>(zmq_msg_data(&msg));
    //    for (int i = 0; i < zmq_msg_size(&msg); i++)
    //    {
    //    	std::cout << std::hex << msgByteArray[i] << ":";
    //
    //    }
    //    std::cout << std::endl;

    // set group
    check(zmq_msg_set_group(&msg, "TestMCGroup"), "zmq_msg_set_group", false);

    // send
    int numBytesSend = zmq_msg_send(&msg, socket, 0);
    if (numBytesSend == -1)
    {
        std::cerr << "zmq_msg_send was unsuccessfull: " << errno << " - " << zmq_strerror(errno) << std::endl;
        check(zmq_msg_close(&msg), "zmq_msg_close", false);
    }
    else
    {
        std::cout << numBytesSend << " bytes sent." << std::endl;
    }
}

int main(int argc, char **argv)
{
    auto ctx = zmq_ctx_new();
    assert(ctx);

    auto socket = zmq_socket(ctx, ZMQ_RADIO);
    check(zmq_connect(socket, "udp://224.0.0.1:5555"), "zmq_connect", true);

    send(socket);

    check(zmq_close(socket), "zmq_close", true);
    check(zmq_ctx_term(ctx), "zmq_ctx_term", true);
}

#include <discovery_msgs/beacon.capnp.h>
#include <zmq.h>

#include <uuid/uuid.h>

#include <assert.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <iostream>

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

int main(int argc, char **argv)
{
    auto ctx = zmq_ctx_new();
    assert(ctx);

    auto socket = zmq_socket(ctx, ZMQ_RADIO);
    check(zmq_connect(socket, "udp://224.0.0.1:5555"), "zmq_connect", true);

    ::capnp::MallocMessageBuilder msgBuilder;
    discovery_msgs::Beacon::Builder beaconMsgBuilder = msgBuilder.initRoot<discovery_msgs::Beacon>();

    // set content
    uuid_t uuid;
    uuid_generate(uuid);

    beaconMsgBuilder.setIp("192.186.0.1");
    beaconMsgBuilder.setPort(6666);
    beaconMsgBuilder.setUuid(kj::arrayPtr(uuid, sizeof(uuid)));

    zmq_msg_t msg;
    auto byteArray = capnp::messageToFlatArray(msgBuilder).asBytes();

    ::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(capnp::messageToFlatArray(msgBuilder));

        auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();

        std::cout << "Received Message: " << beacon.toString().flatten().cStr() << std::endl;

    check(zmq_msg_init_data(&msg, byteArray.begin(), byteArray.size(), NULL, NULL), "zmq_msg_init_data", false);

    // set group
    check(zmq_msg_set_group(&msg, "TestMCGroup"), "zmq_msg_set_group", false);

    /* uncomment for printing bytes */
//	auto msgByteArray = reinterpret_cast<uintptr_t*>(zmq_msg_data(&msg));
//	for (int i = 0; i < zmq_msg_size(&msg); i++)
//	{
//		printf("%02X:", msgByteArray[i]);
//	}
//	printf("\n");

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

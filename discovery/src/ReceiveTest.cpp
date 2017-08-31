#include <discovery_msgs/beacon.capnp.h>
#include <zmq.h>

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
    // zmq stuff
    auto ctx = zmq_ctx_new();
    assert(ctx);

    // create socket
    auto socket = zmq_socket(ctx, ZMQ_DISH);
    assert(socket);

    // bind socket
    check(zmq_bind(socket, "udp://224.0.0.1:5555"), "zmq_bind", true);
    check(zmq_join(socket, "TestMCGroup"), "zmq_join", true);

    zmq_msg_t msg;
    check(zmq_msg_init(&msg), "zmq_msg_init", true);
    int nbytes = zmq_msg_recv(&msg, socket, 0);

    std::cout << "Received " << nbytes << " bytes!" << std::endl;

    // Received message must contain an integral number of words.
    assert(zmq_msg_size(&msg) % sizeof(capnp::word) == 0);
    auto num_words = zmq_msg_size(&msg) / sizeof(capnp::word);

    std::cout << "The zmq_msg_size(&msg) is " << zmq_msg_size(&msg) << ", which are "  << num_words << " words."<< std::endl;

    if (reinterpret_cast<uintptr_t>(zmq_msg_data(&msg)) % sizeof(capnp::word) == 0)
    {
        std::cout << "The message is memory aligned." << std::endl;
    }
    else
    {
        std::cout << "The message is not memory aligned." << std::endl;
    }

    /* uncomment for printing the bytes in hex */
	//    auto msgByteArray = reinterpret_cast<char *>(zmq_msg_data(&msg));
    //    for (int i = 0; i < zmq_msg_size(&msg); i++)
    //    {
    //        printf("%02X:", msgByteArray[i]);
    //    }
    //    printf("\n");

    auto wordArray = kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(zmq_msg_data(&msg)), num_words);

    ::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(wordArray);

    auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();

    std::cout << "Received Message: " << beacon.toString().flatten().cStr() << std::endl;

    check(zmq_msg_close(&msg), "zmq_msg_close", false);
}

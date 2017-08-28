#include <discovery_msgs/beacon.capnp.h>
#include <zmq.h>

#include <assert.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <iostream>

int main(int argc, char **argv)
{
    // zmq stuff
    auto ctx = zmq_ctx_new();
    assert(ctx);

    // create socket
    auto socket = zmq_socket(ctx, ZMQ_DISH);
    assert (socket);

    // bind socket
    //int rc = zmq_bind(socket, "udp://192.168.122.161:5555");
    int rc = zmq_bind(socket, "udp://224.0.0.1:5555");
    if (rc != 0)
    {
        std::cerr << "Error: " << zmq_strerror(errno) << std::endl;
        assert(rc == 0);
    }

    rc = zmq_join (socket, "TV");
    if (rc != 0)
	{
		std::cerr << "Error: " << zmq_strerror(errno) << std::endl;
		assert(rc == 0);
	}

    char buf [256];
    int nbytes = zmq_recv (socket, buf, 256, 0); assert (nbytes != -1);
    std::cerr << "Error: " << zmq_strerror(errno) << std::endl;

    std::cout << "Test: " << nbytes << " bytes received!" << std::endl;
}

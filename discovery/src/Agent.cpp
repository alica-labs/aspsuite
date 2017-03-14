#include "discovery/Agent.h"

#include <zmq.h>

#include <assert.h>
#include <chrono>
#include <iostream>
#include <string.h>
#include <unistd.h>

//#define DEBUG_AGENT

namespace discovery
{

bool Agent::operating = true;

Agent::Agent(std::string name, bool sender)
    : Worker(name)
    , sender(sender)
    , socket(nullptr)
{
    // generate
    uuid_generate(this->uuid);

    // zmq stuff
    this->ctx = zmq_ctx_new();
    assert(ctx);

    if (sender)
    {
        this->setupSendUDPMulticast();
    }
    else
    {
        this->setupReceiveUDPMulticast();
    }
}

void Agent::setupReceiveUDPMulticast()
{
    this->socket = zmq_socket(ctx, ZMQ_DISH);

    int rc = zmq_bind(socket, "udp://224.0.0.1:5555");
    if (rc != 0)
    {
        std::cout << "Error: " << zmq_strerror(errno) << std::endl;
        assert(rc == 0);
    }

    // Joining
    rc = zmq_join(socket, "Movies");
    if (rc != 0)
    {
        std::cout << "Error: " << zmq_strerror(errno) << std::endl;
        assert(rc == 0);
    }
}

void Agent::setupSendUDPMulticast()
{
    this->socket = zmq_socket(ctx, ZMQ_RADIO);

    // Connecting
    int rc = zmq_connect(socket, "udp://224.0.0.1:5555");
    if (rc != 0)
    {
        std::cout << "Error: " << zmq_strerror(errno) << std::endl;
        assert(rc == 0);
    }
}

Agent::~Agent()
{
    int rc = zmq_close(socket);
    if (rc != 0)
    {
        strerror(errno);
        assert(rc == 0);
    }

    rc = zmq_ctx_term(this->ctx);
    if (rc != 0)
    {
        strerror(errno);
        assert(rc == 0);
    }
}

void Agent::run()
{
    // this->checkZMQVersion();
    // this->testUUIDStuff();

    if (this->sender)
    {
#ifdef DEBUG_AGENT
        std::cout << "Error State before sending: " << zmq_strerror(errno) << std::endl;
#endif
        send();
#ifdef DEBUG_AGENT
        std::cout << "Error State after sending: " << zmq_strerror(errno) << std::endl;
#endif
    }
    else
    {
#ifdef DEBUG_AGENT
        std::cout << "Error State before receiving: " << zmq_strerror(errno) << std::endl;
#endif
        receive();
#ifdef DEBUG_AGENT
        std::cout << "Error State after receiving: " << zmq_strerror(errno) << std::endl;
#endif
    }
}

void Agent::send()
{
    zmq_msg_t msg;
    int rc = msg_send(&msg, this->socket, "Movies", "Godfather");
    std::cout << "Sended " << rc << " chars." << std::endl;
    assert(rc == 9);
}

void Agent::receive()
{
    zmq_msg_t msg;
    int rc = msg_recv_cmp(&msg, this->socket, "Movies", "Godfather");
    std::cout << "Received " << rc << " chars." << std::endl;
    assert(rc == 9);
}

int Agent::msg_send(zmq_msg_t *msg_, void *s_, const char *group_, const char *body_)
{
    int rc = zmq_msg_init_size(msg_, strlen(body_));
    if (rc != 0)
    {
    	std::cout << "Agent::msg_send: Error after init msg: " << zmq_strerror(errno) << std::endl;
        return rc;
    }

    std::cout << "Agent::msg_send: Message could be initialised. Size is " << strlen(body_) << std::endl;

    memcpy(zmq_msg_data(msg_), body_, strlen(body_));

    rc = zmq_msg_set_group(msg_, group_);
    if (rc != 0)
    {
        std::cout << "Agent::msg_send: Failure by setting the group of a message: " << zmq_strerror(errno) << std::endl;
        zmq_msg_close(msg_);
        return rc;
    }

    rc = zmq_msg_send(msg_, s_, 0);
    std::cout << "Agent::msg_send: Send " << rc << " chars." << std::endl;

    zmq_msg_close(msg_);

    return rc;
}

int Agent::msg_recv_cmp(zmq_msg_t *msg_, void *s_, const char *group_, const char *body_)
{
    int rc = zmq_msg_init(msg_);
    if (rc != 0)
    {
    	std::cout << "Agent::msg_recv_cmp: Error after init msg: " << zmq_strerror(errno) << std::endl;
        return -1;
    }

    int recv_rc = zmq_msg_recv(msg_, s_, 0);
    if (recv_rc == -1)
    {
    	std::cout << "Agent::msg_recv_cmp: Error after receive msg: " << zmq_strerror(errno) << std::endl;
        return -1;
    }

    if (strcmp(zmq_msg_group(msg_), group_) != 0)
    {
    	std::cout << "Agent::msg_recv_cmp: Error after compare group: " << zmq_strerror(errno) << std::endl;
        zmq_msg_close(msg_);
        return -1;
    }

    char *body = (char *)malloc(sizeof(char) * (zmq_msg_size(msg_) + 1));
    memcpy(body, zmq_msg_data(msg_), zmq_msg_size(msg_));
    body[zmq_msg_size(msg_)] = '\0';

    if (strcmp(body, body_) != 0)
    {
    	std::cout << "Agent::msg_recv_cmp: Message does not fit!" << std::endl;
        zmq_msg_close(msg_);
        return -1;
    }

    zmq_msg_close(msg_);
    free(body);
    return recv_rc;
}

void Agent::testUUIDStuff()
{
    uuid_generate(this->uuid);

    // unparse (to string)
    char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
    uuid_unparse_lower(uuid, uuid_str);
    printf("generate uuid=%s\n", uuid_str);

    // parse (from string)
    uuid_t uuid2;
    uuid_parse(uuid_str, uuid2);

    // compare (rv == 0)
    int rv;
    rv = uuid_compare(uuid, uuid2);
    printf("uuid_compare() result=%d\n", rv);

    // compare (rv == 1)
    uuid_t uuid3;
    uuid_parse("1b4e28ba-2fa1-11d2-883f-0016d3cca427", uuid3);
    rv = uuid_compare(uuid, uuid3);
    printf("uuid_compare() result=%d\n", rv);

    // is null? (rv == 0)
    rv = uuid_is_null(uuid);
    printf("uuid_null() result=%d\n", rv);

    // is null? (rv == 1)
    uuid_clear(uuid);
    rv = uuid_is_null(uuid);
    printf("uuid_null() result=%d\n", rv);
}

void Agent::checkZMQVersion()
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    std::cout << "Major: " << major << " Minor: " << minor << " Patch: " << patch << std::endl;
}

void Agent::sigIntHandler(int sig)
{
    operating = false;
}
}

int main(int argc, char **argv)
{
    discovery::Agent *agent;
    if (argc > 1 && strcmp(argv[1], "sender") == 0)
    {
        std::cout << "Creating sending agent!" << std::endl;
        agent = new discovery::Agent("Sender", true);
    }
    else
    {
        std::cout << "Creating receiving agent!" << std::endl;
        agent = new discovery::Agent("Receiver", false);
    }

    agent->setDelayedStartMS(std::chrono::milliseconds(0));
    agent->setIntervalMS(std::chrono::milliseconds(1000));

    agent->start();

    // register ctrl+c handler
    signal(SIGINT, discovery::Agent::sigIntHandler);

    // start running
    while (discovery::Agent::operating)
    {
        sleep(1);
    }

    agent->stop();

    delete agent;
    return 0;
}

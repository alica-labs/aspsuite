#include "discovery/Agent.h"

#include <discovery_msgs/beacon.capnp.h>
#include <zmq.h>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <assert.h>
#include <chrono>
#include <iostream>
#include <string.h>
#include <unistd.h>

// Network stuff
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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

    this->getWirelessAddress();

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

/**
 * This method should store the IP address of the W-LAN interface, if available.
 * @return True, if IP address is determined, false otherwise.
 */
bool Agent::getWirelessAddress()
{
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        // filters for 'w'ireless interfaces with ipv4 addresses
        if (ifa->ifa_name[0] == 'w' && ifa->ifa_addr->sa_family == AF_INET)
        { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            // char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, this->wirelessIpAddress, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, this->wirelessIpAddress);
            if (ifAddrStruct != NULL)
                freeifaddrs(ifAddrStruct);
            return true;
        }
    }
    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);
    return false;
}

void Agent::send()
{
    ::capnp::MallocMessageBuilder message;

    discovery_msgs::Beacon::Builder beacon = message.initRoot<discovery_msgs::Beacon>();
    beacon.setIp(this->wirelessIpAddress);
    beacon.setPort(6666);
    ::capnp::Data::Builder uuidBuilder = beacon.initUuid(16);
    uuidBuilder[0] = this->uuid;

    auto segments = message.getSegmentsForOutput();

    auto it = segments.begin();
    auto i = segments.size();
    assert(i != 0);
    while (--i != 0)
    {
        zmq_send_const(this->socket, reinterpret_cast<char const *>(&(*it)[0]), it->size() * sizeof(capnp::word),
                       ZMQ_SNDMORE);
        ++it;
    }
    zmq_send_const(this->socket, reinterpret_cast<char const *>(&(*it)[0]), it->size() * sizeof(capnp::word), 0);

    //    zmq_msg_t msg;
    //    int rc = msg_send(&msg, this->socket, "Movies", "Godfather");
    //    std::cout << "Sended " << rc << " chars." << std::endl;
    //    assert(rc == 9);
}

int Agent::msg_send(zmq_msg_t *msg_, void *s_, const char *group_, const char *body_)
{
    int rc = zmq_msg_init_size(msg_, strlen(body_));
    if (rc != 0)
    {
        std::cout << "Agent::msg_send: Error after init msg: " << zmq_strerror(errno) << std::endl;
        return rc;
    }

    memcpy(zmq_msg_data(msg_), body_, strlen(body_));

    rc = zmq_msg_set_group(msg_, group_);
    if (rc != 0)
    {
        std::cout << "Agent::msg_send: Failure by setting the group of a message: " << zmq_strerror(errno) << std::endl;
        zmq_msg_close(msg_);
        return rc;
    }

    rc = zmq_msg_send(msg_, s_, 0);

    zmq_msg_close(msg_);

    return rc;
}

void Agent::receive()
{
    zmq_msg_t msg;
    segments_.clear();
    while (true)
    {
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, this->socket, 0);

        // Received message must contain an integral number of words.
        assert(zmq_msg_size(&msg) % sizeof(capnp::word) == 0);
        auto num_words = zmq_msg_size(&msg) / sizeof(capnp::word);

        if (reinterpret_cast<uintptr_t>(&msg) % sizeof(capnp::word) == 0)
        {
        	std::cout << "Agent: Receive(): Message is aligned! " << std::endl;
        	segments_.push_back(kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(&msg), num_words));
        }
        else
        {
        	std::cout << "Agent: Receive(): Message is NOT aligned! " << std::endl;
        	// TODO :D
        }

        capnp::SegmentArrayMessageReader reader(kj::ArrayPtr<kj::ArrayPtr<capnp::word const>>(&segments_[0], segments_.size()));

        capnp::MallocMessageBuilder builder;
        auto bla = reader.getRoot<discovery_msgs::Beacon>();

        if (!zmq_msg_more(&msg))
        {
            zmq_msg_close(&msg);
            break;
        }
        else
        {
            zmq_msg_close(&msg);
        }
    }
}

kj::ArrayPtr<kj::ArrayPtr<capnp::word const>> Agent::genericReceive()
{
    do
    {
        assert(str.size() % sizeof(capnp::word) == 0); // Received message must contain an integral number of words.
        auto num_words = str.size() / sizeof(capnp::word);
        char *buf = &str[0];

        if (reinterpret_cast<uintptr_t>(buf) % sizeof(capnp::word) == 0)
        {
            // String buffer is word-aligned, point directly at the start of the string.
            segments_.push_back(kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(buf), num_words));
        }
        else
        {
            // String buffer is not word-aligned, make a copy and point at that.
            unique_ptr<capnp::word[]> words(new capnp::word[num_words]);
            memcpy(words.get(), buf, str.size());
            segments_.push_back(kj::ArrayPtr<capnp::word const>(&words[0], num_words));
            copied_parts_.push_back(move(words));
        }
    } while (s_.has_more_parts());

    return kj::ArrayPtr<kj::ArrayPtr<capnp::word const>>(&segments_[0], segments_.size());
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

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
#include <sys/types.h>

#define DEBUG_AGENT

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

    if (this->sender)
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

    // Binding
    // check(zmq_bind(socket, "udp://192.168.122.161:5555"), "zmq_bind");
    check(zmq_bind(socket, "udp://224.0.0.1:5555"), "zmq_bind");

    // Join ZMQ Publisher-Grou
    check(zmq_join(this->socket, "TestMCGroup"), "zmq_join");
}

void Agent::setupSendUDPMulticast()
{
    this->socket = zmq_socket(ctx, ZMQ_RADIO);

    // Connecting
    // check(zmq_connect(socket, "udp://192.168.122.161:5555"), "zmq_connect", true);
    check(zmq_connect(socket, "udp://224.0.0.1:5555"), "zmq_connect", true);
}

Agent::~Agent()
{
    check(zmq_close(socket), "zmq_close", true);
    check(zmq_ctx_term(this->ctx), "zmq_ctx_term", true);
}

void Agent::run()
{
    // this->checkZMQVersion();
    // this->testUUIDStuff();

    if (!this->getWirelessAddress())
    {
        std::cerr << "Agent: No WLAN-Address available! " << std::endl;
        return;
    }

    if (this->sender)
    {
        // sendMultiPartZeroCopy();
        send();
    }
    else
    {
        receive();
    }
}

void Agent::send()
{
    // Cap'n Proto: create proto message

    // init builder
    ::capnp::MallocMessageBuilder msgBuilder;
    discovery_msgs::Beacon::Builder beaconMsgBuilder = msgBuilder.initRoot<discovery_msgs::Beacon>();

    // set content
    beaconMsgBuilder.setIp(this->wirelessIpAddress);
    beaconMsgBuilder.setPort(6666);
    beaconMsgBuilder.setUuid(kj::arrayPtr(this->uuid, sizeof(this->uuid)));

#ifdef DEBUG_AGENT
    std::cout << "Agent:send(): Message (Size: "
              << ") to send: " << beaconMsgBuilder.toString().flatten().cStr() << std::endl;
#endif

    // ZMQ: send proto message via zmq message

    // copy content
    zmq_msg_t msg;
    kj::ArrayPtr<uint8_t> byteArray = capnp::messageToFlatArray(msgBuilder).asBytes();
    check(zmq_msg_init_data(&msg, byteArray.begin(), byteArray.size(), NULL, NULL), "zmq_msg_init_data");

    // set group
    check(zmq_msg_set_group(&msg, "TestMCGroup"), "zmq_msg_set_group");

    // send
    check(zmq_msg_send(&msg, this->socket, 0), "zmq_msg_send");
}

/**
 * Checks the return code and reports an error if present.
 * If abortIfError is true, it also aborts the process.
 */
void Agent::check(int returnCode, std::string methodName, bool abortIfError)
{
    if (returnCode != 0)
    {
        std::cerr << methodName << " returned: " << errno << " - " << zmq_strerror(errno) << std::endl;
        if (abortIfError)
            assert(returnCode);
    }
}

void Agent::sendMultiPartZeroCopy()
{
    ::capnp::MallocMessageBuilder msgBuilder;

    discovery_msgs::Beacon::Builder beacon = msgBuilder.initRoot<discovery_msgs::Beacon>();
    beacon.setIp(this->wirelessIpAddress);
    beacon.setPort(6666);
    std::cout << "Agent: send(): Size of UUID is " << sizeof(this->uuid) << std::endl;
    beacon.setUuid(kj::arrayPtr(this->uuid, sizeof(this->uuid)));

    std::cout << "Agent:send(): Message (Size: "
              << ") to send: " << beacon.toString().flatten().cStr() << std::endl;

    auto segments = msgBuilder.getSegmentsForOutput();

    auto it = segments.begin();
    int i = segments.size();
    assert(i != 0);
    while (--i != 0)
    {
        std::cout << "Agent:send(): Size: " << it->size() * sizeof(capnp::word) << std::endl;
        zmq_send_const(this->socket, reinterpret_cast<char const *>(&(*it)[0]), it->size() * sizeof(capnp::word),
                       ZMQ_SNDMORE);
        ++it;
    }
    std::cout << "Agent:send(): Size: " << it->size() * sizeof(capnp::word) << std::endl;
    zmq_send_const(this->socket, reinterpret_cast<char const *>(&(*it)[0]), it->size() * sizeof(capnp::word), 0);
}

void Agent::receive()
{
    zmq_msg_t msg;

	check(zmq_msg_init(&msg), "zmq_msg_init", true);
	int nbytes = zmq_msg_recv(&msg, this->socket, 0);
	std::cout << "Agent: receive(): nbytes: " << nbytes << std::endl;

	// Received message must contain an integral number of words.
	assert(zmq_msg_size(&msg) % sizeof(capnp::word) == 0);
	auto num_words = zmq_msg_size(&msg) / sizeof(capnp::word);
	std::cout << "Agent: receive(): num_words: " << num_words << std::endl;

	auto wordArray = kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(zmq_msg_data(&msg)), num_words);
	std::cout << "Agent: receive(): wordArray size: " << wordArray.size() << std::endl;


	::capnp::FlatArrayMessageReader msgReader = ::capnp::FlatArrayMessageReader(wordArray);

    auto beacon = msgReader.getRoot<discovery_msgs::Beacon>();

    std::cout << "Agent: receive(): " << beacon.toString().flatten().cStr() << std::endl;

	check(zmq_msg_close(&msg), "zmq_msg_close");
}

// kj::ArrayPtr<kj::ArrayPtr<capnp::word const>> Agent::genericReceive()
//{
//    do
//    {
//        assert(str.size() % sizeof(capnp::word) == 0); // Received message must contain an integral number of words.
//        auto num_words = str.size() / sizeof(capnp::word);
//        char *buf = &str[0];
//
//        if (reinterpret_cast<uintptr_t>(buf) % sizeof(capnp::word) == 0)
//        {
//            // String buffer is word-aligned, point directly at the start of the string.
//            segments_.push_back(kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const *>(buf),
//            num_words));
//        }
//        else
//        {
//            // String buffer is not word-aligned, make a copy and point at that.
//            unique_ptr<capnp::word[]> words(new capnp::word[num_words]);
//            memcpy(words.get(), buf, str.size());
//            segments_.push_back(kj::ArrayPtr<capnp::word const>(&words[0], num_words));
//            copied_parts_.push_back(move(words));
//        }
//    } while (s_.has_more_parts());
//
//    return kj::ArrayPtr<kj::ArrayPtr<capnp::word const>>(&segments_[0], segments_.size());
//}

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

/**
 * This method should store the IP address of the W-LAN interface, if available.
 * @return True, if IP address is determined, false otherwise.
 */
bool Agent::getWirelessAddress()
{
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;

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
            int error = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), this->wirelessIpAddress, NI_MAXHOST,
                                    NULL, 0, NI_NUMERICHOST);
            if (error != 0)
            {
                std::cout << "Agent:getWirelessAddress: getnameinfo() failed: " << gai_strerror(error) << std::endl;
                exit(EXIT_FAILURE);
            }

            if (ifAddrStruct != NULL)
                freeifaddrs(ifAddrStruct);
            return true;
        }
        else
        {
            // std::cout << "Agent::getWirelessAddress: Interface Name: " << ifa->ifa_name
            //          << " Protocol Family: " << ifa->ifa_addr->sa_family << std::endl;
        }
    }
    if (ifAddrStruct != NULL)
        freeifaddrs(ifAddrStruct);
    return false;
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

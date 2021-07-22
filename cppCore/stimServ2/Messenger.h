#pragma once

#include <zmq.hpp>
#include <thread>
#include <memory>
#include <string>
#include <functional>

// #include "IMessenger.h"


class Listener {
private:
    zmq::context_t _context;
    zmq::socket_t  _socket;

public:
    Listener(const std::string url);
    ~Listener();

    void listen();
};

class Requester {
    private:
    zmq::context_t _context;
    zmq::socket_t  _socket;

public:
    Requester(const std::string url);
    ~Requester();

    void send(std::string &s);
    void send(const char *s);
};
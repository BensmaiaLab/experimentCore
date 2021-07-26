#pragma once

#include <zmq.hpp>
#include <thread>
#include <memory>
#include <string>
#include <functional>

// #include "IMessenger.h"


class Server {
private:
    zmq::context_t _context;
    zmq::socket_t  _socket;

public:
    Server(const std::string url);
    ~Server();

    void listen();
};

class Client {
    private:
    zmq::context_t _context;
    zmq::socket_t  _socket;

public:
    Client(const std::string url);
    ~Client();

    void send(std::string &s);
    void send(const char *s);
};
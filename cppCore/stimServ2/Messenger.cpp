#include "pch.h"
#include <string>
#include "Messenger.h"

/*
 Based on: https://blog.pramp.com/inter-thread-communication-in-c-futures-promises-vs-sockets-aeebfffd2107
*/


// Listener
Listener::Listener(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REP)
{
    this->_socket.bind(url);
}

Listener::~Listener() {}

void Listener::listen() {
    while (1) {
        zmq::message_t request;
        auto response = this->_socket.recv(request, zmq::recv_flags::none);
        logInfo << "Received Hello: " << request.str();
        Sleep(1000); //  Do some 'work'

        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        this->_socket.send(reply, zmq::send_flags::none);
    }
}


// Requester
Requester::Requester(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REQ)
{
    this->_socket.bind(url);
}

Requester::~Requester() {}

void Requester::send(std::string &s) {
    zmq::message_t reply(s.length());
    memcpy(reply.data(), s.c_str(), s.length());
    this->_socket.send(reply, zmq::send_flags::none);
}

void Requester::send(const char *s) {
    zmq::message_t reply(strlen(s));
    memcpy(reply.data(), s, strlen(s));
    this->_socket.send(reply, zmq::send_flags::none);
}



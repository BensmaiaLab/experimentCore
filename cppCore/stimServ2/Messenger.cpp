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
    logInfo << "Listener bound to " << url;
}

Listener::~Listener() {}

void Listener::listen() {
    while (1) {
        zmq::message_t request;
        auto res1= this->_socket.recv(request, zmq::recv_flags::none);
        logDebug << "Received Hello: " << request.str();
        Sleep(1000); //  Do some 'work'

        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        auto res2 = this->_socket.send(reply, zmq::send_flags::none);
    }
}


// Requester
Requester::Requester(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REQ)
{
    try {
        this->_socket.connect(url);
    } catch (zmq::error_t::exception) {
        logError << "Failed to connect Requester socket to url " << url;
        this->_socket.close();
    }
    logInfo << "Requester connected to " << url;
}

Requester::~Requester() {}

void Requester::send(std::string &s) {
    zmq::message_t msg(s.length());
    memcpy(msg.data(), s.c_str(), s.length());
    auto response = this->_socket.send(msg, zmq::send_flags::none);
}

void Requester::send(const char *s) {
    zmq::message_t msg(strlen(s));
    //memcpy(msg.data(), s, strlen(s));
    memcpy(msg.data(), "hello", 5);
    auto response = this->_socket.send(msg, zmq::send_flags::none);
    if (response != 0) {
        logError << "failed to send with errno " << response;
    }
    logInfo << "Sent hello.";
}



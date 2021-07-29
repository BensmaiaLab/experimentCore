#include "pch.h"
#include <string>
#include "Messenger.h"

/*
 Based on: https://blog.pramp.com/inter-thread-communication-in-c-futures-promises-vs-sockets-aeebfffd2107
*/


// Listener
Server::Server(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REP)
{
    // Get version
    auto [vMa, vMi, vP] = zmq::version();
    logInfo << "Using ZeroMQ version " << vMa << "." << vMi << "." << vP << ".";

    try {
        this->_socket.bind(url);
    } catch (zmq::error_t::exception) {
        logError << "Failed to bind Listener socket to url " << url;
        this->_socket.close();
    }
    logInfo << "Listener bound to " << url;
}

Server::~Server() {}

/*
    Takes a callback function to a message processor.
    Callback can return a "stop" string which will
    tell the listen loop to shutdown.
*/
void Server::listen(std::function<std::string(std::string)> f) {
    while (1) {
        zmq::message_t request;
        zmq::recv_result_t res1 = this->_socket.recv(request, zmq::recv_flags::none);
        
        // Call message processor:
        std::string resp = f(request.to_string());
        logInfo << "server response is: " << resp;
        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        zmq::send_result_t res2 = this->_socket.send(reply, zmq::send_flags::none);
        // Have to reply first before we shutdown...
        if (resp == std::string("stop")) {
            logInfo << "Server got stop.";
            break;
        }
    }
    logInfo << "server.listen() shutting down.";
    return;
}


// Client
Client::Client(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REQ)
{
    try {
        this->_socket.connect(url);
    } catch (zmq::error_t::exception e) {
        logError << "Failed to connect Client socket to url " << url << " with exception " << e.what();
        this->_socket.close();
    }
    logInfo << "Client connected to " << url;
}

Client::~Client() {}

void Client::send(std::string &s) {
    zmq::message_t msg(s.length());
    memcpy(msg.data(), s.c_str(), s.length());
    zmq::send_result_t response = this->_socket.send(msg, zmq::send_flags::none);
}

void Client::send(const char *s) {
    zmq::message_t msg(strlen(s));
    memcpy(msg.data(), s, strlen(s));
    //memcpy(msg.data(), "hello", 5);
    //logInfo << "attemping to send message: " << msg.to_string();
    try {
        zmq::send_result_t response = this->_socket.send(msg, zmq::send_flags::none);
    } catch (zmq::error_t::exception e) {
        logError << "failed to send: " << e.what();
    }

    // Now we need to get the reply:
    zmq::message_t request;
    zmq::recv_result_t res1 = this->_socket.recv(request, zmq::recv_flags::none);
    //logInfo << "Received: " << request.str();
}



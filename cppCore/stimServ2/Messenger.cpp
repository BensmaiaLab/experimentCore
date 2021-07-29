#include "pch.h"
#include <string>
#include "Messenger.h"

/*
 Based on zmq. 

*/

void printZMQver(){
    auto [vMa, vMi, vP] = zmq::version();
    logInfo << "Using ZeroMQ version " << vMa << "." << vMi << "." << vP << ".";
}

/* Basic listen server implementation with zmq. */
Server::Server(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REP)
{
    printZMQver();
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
        std::string req = f(request.to_string());
        //logInfo << "server response is: " << resp;
        zmq::message_t reply(5);
        memcpy(reply.data(), "World", 5);
        zmq::send_result_t res2 = this->_socket.send(reply, zmq::send_flags::none);
        // Have to reply first before we shutdown...
        if (req == std::string("stop")) break;
    }
    logInfo << "server shutting down.";
    return;
}


// Client
Client::Client(const std::string url):
    _context(1), // n io threads (1 is sane default)
    _socket(this->_context, ZMQ_REQ)
{
    // Wait 1 second for replies before failing so we don't block indef on receive after send
    this->_socket.set(zmq::sockopt::rcvtimeo, 1000);
    try {
        this->_socket.connect(url);
    } catch (zmq::error_t::exception e) {
        logError << "Failed to connect Client socket to url " << url << " with exception " << e.what();
        this->_socket.close();
    }
    logInfo << "Client connected to " << url;
}

Client::~Client() {}

std::string Client::send(const std::string s) {
    zmq::message_t msg(s.length());
    memcpy(msg.data(), s.data(), s.length());
    try {
        zmq::send_result_t response = this->_socket.send(msg, zmq::send_flags::none);
    } catch (zmq::error_t::exception e) {
        logError << "failed to send: " << e.what();
    }
    zmq::message_t reply;
    zmq::recv_result_t res1 = this->_socket.recv(reply, zmq::recv_flags::none);
    return reply.str();
}
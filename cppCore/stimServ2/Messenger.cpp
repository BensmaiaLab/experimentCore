#include "pch.h"
#include <string>
#include "Messenger.h"

/*
 Based on: https://blog.pramp.com/inter-thread-communication-in-c-futures-promises-vs-sockets-aeebfffd2107
*/

Messenger::Messenger() {
    // Playing around with wrapping in unique_ptr for better safety
    //this->_context = std::make_unique<zmq::context_t>(zmq_ctx_new());
    this->_context = zmq_ctx_new();
    
    //this->_socket = std::make_unique<zmq::socket_t>(zmq_socket(this->_context, ZMQ_REP));
    this->_socket = zmq_socket(this->_context, ZMQ_REP);
    int rc = zmq_bind(this->_socket, "tcp://*:5555");
    assert (rc == 0); //Check and make sure we bound, error handling?
}

Messenger::~Messenger() {}

void Messenger::receive() {
    while (1) {
        char buffer [10];
        zmq_recv (this->_socket, buffer, 10, 0);
        logInfo << "Received Hello";
        Sleep(1000);          //  Do some 'work'
        zmq_send (this->_socket, "World", 5, 0);
    }
}


void Messenger::send(std::string &s) {
    zmq_send(this->_socket, s.c_str(), s.length(), 0);
}
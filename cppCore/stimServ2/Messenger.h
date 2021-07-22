#pragma once

#include <zmq.hpp>
#include <thread>
#include <memory>
#include <string>
#include <functional>

#include "IMessenger.h"

class Messenger: public IMessenger {
private:
    //zmq context (necessary for sockets to work)
    void * _context;
    //std::unique_ptr<zmq::context_t> _context;

    //socket that will talk to managed background thread's
    //socket
    void * _socket;
    //std::unique_ptr<zmq::socket_t> _socket;
    
    //managed thread
    //std::unique_ptr<std::thread> _thread;

public:
    //constructor (takes in fptr to a "reactor" function that "reacts"
    //to a string message)
    //Messenger(const std::function<void(std::string)>& reactor);

    // Normal constructor
    Messenger();
    
    //destructor (stops and waits for managed thread to exit)
    ~Messenger();

    //interface method
    void receive();

    void send(std::string &s);


};
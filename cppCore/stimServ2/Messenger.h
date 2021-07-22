#pragma once

#include <zmq.hpp>
#include <thread>
#include <memory>
#include <string>
#include <functional>

#include "IMessenger.h"

class Messenger: public IMessenger {
private:
    zmq::context_t _context;
    zmq::socket_t  _socket;
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
    void send(const char *s);

};
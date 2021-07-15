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
    std::unique_ptr<zmq::context_t> m_context;

    //socket that will talk to managed background thread's
    //socket
    std::unique_ptr<zmq::socket_t> m_manager;
    
    //managed thread
    std::unique_ptr<std::thread> m_thread;

    //stop message
    const static std::string STOP;
public:
    //constructor (takes in fptr to a "reactor" function that "reacts"
    //to a string message)
    Messenger(const std::function<void(std::string)>& reactor);
    
    //interface method
    void processMessage(std::string msg);

    //destructor (stops and waits for managed thread to exit)
    ~Messenger();

};
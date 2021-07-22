#pragma once
#include <string>

//After instantiation, objects manage a separate thread that can receive messages.
class IMessenger {
public:
    //send msg to the thread this instance manages
    //virtual void processMessage(std::string msg) = 0;
    //to follow RAII, the destructor sends a stop signal (via message) to the managed thread,
	//and can either wait or detach for the thread to exit
    IMessenger() {}
    virtual ~IMessenger() {}
};

// stimServ2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "Messenger.h"

enum class TrialState {
    UNKNOWN,
    Initialized,
    Running,
    Done,
    Error
};

enum class StimulatorState {
    UNKNOWN,
    Initialized,
    Running,
    Done,
    Error
};

class Trial {
public:
    TrialState state;
};


int main() {
    logInfo << "Starting service!";
    Listener listener("tcp://*:5555"); //bind
    Requester requester("tcp://localhost:5555"); //connect
    requester.send("hello");
    listener.listen();
}

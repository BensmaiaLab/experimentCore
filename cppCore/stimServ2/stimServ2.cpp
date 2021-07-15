// stimServ2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"

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


int main()
{
    logInfo << "Starting service!";
    //  Socket to talk to clients

}

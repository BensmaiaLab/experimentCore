// stimServ2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <thread>
#include <chrono>
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


void thread1() {
    logInfo << "thread1 running";
    Listener listener("tcp://*:5555"); //bind
    listener.listen();
}

void thread2() {
    logInfo << "thread2 running";
    Requester requester("tcp://localhost:5555"); //connect
    for (auto i = 0; i < 10; i++) {
        requester.send("hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {
    logInfo << "Starting service!";

    std::thread t1(thread1);
    // Wait a second before starting second thread:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t2(thread2);
    
}

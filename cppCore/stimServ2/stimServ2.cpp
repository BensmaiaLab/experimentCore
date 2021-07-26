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
    logInfo << "running";
    Server listener("tcp://*:25555"); //bind
    listener.listen();
    logInfo << "server shutdown.";
}

void thread2() {
    logInfo << "running";
    Client requester("tcp://localhost:25555"); //connect
    for (auto i = 0; i < 10; i++) {
        logInfo << "hello " << i;
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
    t1.join();
    t2.join();
    logInfo << "Shutdown complete.";
    return 0;
}

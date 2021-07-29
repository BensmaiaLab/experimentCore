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


std::string processMessages(const std::string msg) {
    if      (msg == "hello") return "World";
    else if (msg == "stop")  return "stop";
}

void serverThread(std::function<std::string(std::string)> f) {
    logInfo << "running";
    Server server("tcp://*:25555"); //bind
    server.listen(f);
    logInfo << "server shutdown.";
    return;
}

void clientThread() {
    logInfo << "running";
    Client client("tcp://localhost:25555"); //connect
    for (auto i = 0; i < 10; i++) {
        client.send("hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    client.send("stop");
    logInfo << "client sent stop.";
    return;
}

int main() {
    bool debugMode = false;
    // Program options here
    init_logging(debugMode, (std::string)"StimService2.log"); // bool is "debug mode" TODO: turn into a command line arg
    logInfo << "Starting Stimulator Service.";
    logInfo << "Last built at " << __DATE__ << " " << __TIME__;
    
    // Passing the message processor
    std::function<std::string(std::string)> f = processMessages;
    std::thread t1(serverThread, f);

    // Wait a second before starting client thread to make sure server bound first:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t2(clientThread);
    t2.join();
    logInfo << "Client thread stopped.";
    t1.join();
    logInfo << "Shutdown complete.";
    return 0;
}

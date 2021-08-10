#pragma once

// Treat as RAII, initialization 

class Stimulator {
public:
    enum class State {
        UNKNOWN,
        Initialized,
        Running,
        Done,
        Error
    };

    State state = State::UNKNOWN;

    /* Should confirm State::Initialized before run() */
    void run();
    /* */
    void stop();


};
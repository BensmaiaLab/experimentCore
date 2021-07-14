// stimServ2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"

#include <zmq.h>

int main()
{
    logInfo << "Starting service!";
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        logInfo << "Received Hello";
        Sleep(1000);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}

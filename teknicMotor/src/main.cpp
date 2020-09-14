
// Just test code for the rest of the motor code.
#include <chrono>
#include <thread>
#include "motorAPI.h"

int main(int argc, char* argv[]) {

    BOOST_LOG_TRIVIAL(info) << "main(): Starting Teknic motor interface. Enumerating devices:";
    MotorAPI mapi;

    BOOST_LOG_TRIVIAL(info) << "main(): Now we'll try a move on each. 1 second:";
    // Needs a timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    for (size_t nodeIndex = 0; nodeIndex < mapi.m_nodeCount; nodeIndex++) {
        //TODO: Fix this to call specific node.
        auto myNode = mapi.m_nodes[nodeIndex].get();
        myNode.moveHigh(1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } // for each node
    return 0;
}

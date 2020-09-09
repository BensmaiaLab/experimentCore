#include "motorAPI.h"

int main(int argc, char* argv[]) {

    BOOST_LOG_TRIVIAL(info) << "Starting Teknic motor interface. Enumerating devices:";
    MotorAPI mapi;
    int moveCounts = 1000;


    for (size_t nodeIndex = 0; nodeIndex < mapi.m_nodeCount; nodeIndex++) {
        //TODO: Fix this to call specific node.
        //! mapi.moveNode(mapi.m_nodes[nodeIndex].get(), moveCounts);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } // for each node
    return 0;
}
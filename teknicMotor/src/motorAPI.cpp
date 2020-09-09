/**
 * @file motorAPI.cpp
 * 
 * @brief gRPC API implemented for Teknic Clearpath motors.
 * 
 * @author Danielle MacDonald
 * Contact: dmacd@uchicago.edu
 */


#include "motorAPI.h"


using namespace sFnd;

/**
 * Figure out how many SC4-HUBs are daisy chained ([0-2] per port), and how many
 * servo motors (nodes) are plugged into them. Register them to the SysManager.
 * 
 * Ports are literal COM serial ports, COM6 by default on ours.
 * 
 * Will enable and home all nodes.
 * 
 * (can try to load config file for each motor.)
 */
MotorAPI::MotorAPI(void) {
    // The example just declares it default, says it's singleton
    std::vector<std::string> comHubPorts;

    // But this doesn't show static analysis errors...
    SysManager *m_manager = SysManager::Instance();

    try {
        SysManager::FindComHubPorts(comHubPorts);
        m_portCount = comHubPorts.size();
        BOOST_LOG_TRIVIAL(info) << "Found " << m_portCount << " SC Hubs";
        for (size_t i = 0; i < m_portCount && i < NET_CONTROLLER_MAX; i++)
            m_manager->ComHubPort(i, comHubPorts[i].c_str());
        
        m_manager->PortsOpen(m_portCount);

        // Drop all of our port references into my own array
        for (size_t i = 0; i < m_portCount; i++)
            m_ports.push_back(std::reference_wrapper<IPort>(m_manager->Ports(i)));

        // Initialize nodes. Have to iterate ports, then nodes per port
        //? I only expect one port currently, but this is for safety.
        BOOST_LOG_TRIVIAL(debug) << "Iterating through " << m_portCount << " nodes.";
        for (size_t i = 0; i < m_portCount; i++) { // For each port:
            BOOST_LOG_TRIVIAL(debug) << "Iterating nodes on port " << i << ".";
            IPort &thisPort = m_ports[i].get();

            // Turn off all motors when we initialize the interfaces.
            thisPort.BrakeControl.BrakeSetting(0, BRAKE_ALLOW_MOTION);
            thisPort.BrakeControl.BrakeSetting(1, BRAKE_ALLOW_MOTION);

            auto nodeCntOnPort = thisPort.NodeCount();
            m_nodeCount += nodeCntOnPort;
            BOOST_LOG_TRIVIAL(debug) << "Port,State,Node#: " << thisPort.NetNumber() << thisPort.OpenState() << nodeCntOnPort;
            // Iterate nodes on this port
            for (size_t nodeIndex = 0; nodeIndex < nodeCntOnPort; nodeIndex++) {
                auto thisNode = thisPort.Nodes(nodeIndex);
                thisNode.
                Node wrappedNode = Node(self, thisNode);

                m_nodes.push_back(std::reference_wrapper<Node>(wrappedNode));  //TODO: Push NodeWrapper here instead
            }
        }
        
    } catch (mnErr &theErr) {
        // (defined by the mnErr class)
        BOOST_LOG_TRIVIAL(error) << "MotorAPI() constructor | addr: " << theErr.TheAddr << " | err: " << theErr.ErrorCode << " | msg: " << theErr.ErrorMsg;
    }
}

MotorAPI::~MotorAPI(void){
    BOOST_LOG_TRIVIAL(debug) << "Teknic Shutting down. Disabling nodes, and closing port";
    for (size_t i = 0; i < m_nodeCount; i++) {
        auto thisNode = m_nodes[i].get();
        thisNode.disable();
    }
    m_manager->PortsClose();
    BOOST_LOG_TRIVIAL(debug) << "Teknic Shutdown!";
}


#define HOMING_TIMEOUT		    10000	//The timeout used for homing (ms)
// The timeout used for homing and move operations (in ms)
double MotorAPI::getTimeout(){ return m_manager->TimeStampMsec() + HOMING_TIMEOUT; }




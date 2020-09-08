/**
 * @file motorAPI.cpp
 * 
 * @brief gRPC API implemented for Teknic Clearpath motors.
 * 
 * @author Danielle MacDonald
 * Contact: dmacd@uchicago.edu
 */

#include <stdio.h>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <vector>
#include "motorAPI.h"

using namespace sFnd;

/* The following statements will attempt to enable the node. First, any
shutdowns or NodeStops are cleared, finally the node is enabled */
//! I don't like that this synchronously locks on that while loop
void MotorAPI::enableNode(INode &node) {
    node.Status.AlertsClear();
    node.Motion.NodeStopClear();
    node.EnableReq(true);

    //define a timeout in case the node is unable to enable
    double timeout = getTimeout();
    //This will loop checking on the Real time values of the node's Ready status
    while (!node.Motion.IsReady()) {
        if (mgr->TimeStampMsec() > timeout) {
            BOOST_LOG_TRIVIAL(error) << "Error: Timed out waiting for Node " << node.Info.UserID.Value() << " to enable";
            return;
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Node enabled: " << node.Info.UserID.Value();
}

double MotorAPI::getTimeout(){
    return mgr->TimeStampMsec() + 10000; //The timeout used for homing (in ms)
}

/* Find home position of the node. */
void MotorAPI::homeNode(INode &node){
    if (node.Motion.Homing.HomingValid()) {
        if (node.Motion.Homing.WasHomed()) {
            BOOST_LOG_TRIVIAL(debug) << "Node has already been homed, current position is: \t" << node.Motion.PosnMeasured.Value();
        } else {
            BOOST_LOG_TRIVIAL(debug) << "Node has not been homed.";
        }
        BOOST_LOG_TRIVIAL(info) << "Homing Node now...";
        node.Motion.Homing.Initiate();

        double timeout = getTimeout();    //define a timeout in case the node is unable to enable
        while (!node.Motion.Homing.WasHomed()) {
            if (mgr->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Node completed homing.";
    } else {
        BOOST_LOG_TRIVIAL(error) << "Homing never setup through ClearView. The node cannot be homed: " << node.Info.UserID.Value();
    }
}

/* Diagnostics print. */
void MotorAPI::printNodeDetails(INode &node) {
    std::string nType = "CLEARPATH_SC";
    if ( node.Info.NodeType() == 3) nType = "CLEARPATH_SC_ADV";
    
    BOOST_LOG_TRIVIAL(info) << "  NodeType: " <<  nType;
    BOOST_LOG_TRIVIAL(info) << "     Model: " <<  node.Info.Model.Value();
    BOOST_LOG_TRIVIAL(info) << "  Serial #: " <<  std::to_string(node.Info.SerialNumber.Value()).c_str();
    BOOST_LOG_TRIVIAL(info) << "FW version: " <<  node.Info.FirmwareVersion.Value();
    BOOST_LOG_TRIVIAL(info) << "    userID: " <<  node.Info.UserID.Value();
}


/**
 * Figure out how many SC4-HUBs are daisy chained, and how many motors
 * are plugged into them. Register them to the SysManager.
 * 
 * By default, will try to load config file for each motor. Will enable
 * and home all nodes.
 * ? seems to associate COM ports with SC HUB ports
 * COM portnum (as seen in device manager)
 */
MotorAPI::MotorAPI(void) {
    // The example just declares it default, says it's singleton
    // SysManager mgr;
    std::vector<std::string> comHubPorts;

    // But this doesn't show static analysis errors...
    SysManager *mgr = SysManager::Instance();

    try {
        SysManager::FindComHubPorts(comHubPorts);
        m_portCount = comHubPorts.size();
        BOOST_LOG_TRIVIAL(info) << "Found " << m_portCount << " SC Hubs";
        for (size_t i = 0; i < m_portCount && i < NET_CONTROLLER_MAX; i++)
            mgr->ComHubPort(i, comHubPorts[i].c_str());
        
        mgr->PortsOpen(m_portCount);

        // Drop all of our port references into my own array
        for (size_t i = 0; i < m_portCount; i++)
            m_ports.push_back(std::reference_wrapper<IPort>(mgr->Ports(i)));

        // Initialize nodes. Have to iterate ports, then nodes per port
        size_t nodesOnThisPort;
        BOOST_LOG_TRIVIAL(debug) << "Iterating through " << m_portCount << " nodes.";
        for (size_t i = 0; i < m_portCount; i++) { // For each port:
            BOOST_LOG_TRIVIAL(debug) << "Iterating nodes on port " << i << ".";
            IPort &myPort = m_ports[i].get();
            nodesOnThisPort = myPort.NodeCount();
            m_nodeCount += nodesOnThisPort;
            BOOST_LOG_TRIVIAL(debug) << "Port,State,Node#: " << myPort.NetNumber() << myPort.OpenState() << nodesOnThisPort;
            // Iterate nodes on this port
            for (size_t nodeIndex = 0; nodeIndex < nodesOnThisPort; nodeIndex++) {
                INode &myNode = myPort.Nodes(nodeIndex);
                myNode.EnableReq(false); // Should disable Node before loading config
                mgr->Delay(200);     //? sleep (ms?) to make sure disable is registered?
                //theNode.Setup.ConfigLoad("Config File path");
                printNodeDetails(myNode);
                enableNode(myNode);
                homeNode(myNode);
                // ports.push_back(std::make_unique<IPort>(mgr.Ports(i)));
                m_nodes.push_back(std::reference_wrapper<INode>(myNode));  //! Now we have a reference
            }
        }
        
    } catch (mnErr &theErr) {
        //This statement will print the address of the error, the error code
        // (defined by the mnErr class), as well as the corresponding error message.
        BOOST_LOG_TRIVIAL(error) << "Caught error: addr, err, msg: \n" << theErr.TheAddr << theErr.ErrorCode << theErr.ErrorMsg;
    }
}

MotorAPI::~MotorAPI(void){
    BOOST_LOG_TRIVIAL(info) << "Disabling nodes, and closing port";
    for (size_t i = 0; i < m_portCount; i++) {
            IPort &myPort = mgr->Ports(i);
        for (size_t nodeIndex = 0; nodeIndex < m_nodeCount; nodeIndex++) {
            myPort.Nodes(nodeIndex).EnableReq(false);
        }
    }
    mgr->PortsClose();
}

void MotorAPI::moveNode(INode &node) {
    constexpr auto moveCounts = 10000;
    node.Motion.MoveWentDone();                      //Clear the rising edge Move done register
    node.Port.BrakeControl.BrakeSetting(0, BRAKE_ALLOW_MOTION);
    node.AccUnit(INode::RPM_PER_SEC);                //Set the units for Acceleration to RPM/SEC
    node.VelUnit(INode::RPM);                        //Set the units for Velocity to RPM
    node.Motion.AccLimit = 100000;      // Set Acceleration Limit (RPM/Sec)
    node.Motion.VelLimit = 700;              //Set Velocity Limit (RPM)

    BOOST_LOG_TRIVIAL(info) << "Moving Node";
    node.Motion.MovePosnStart(moveCounts);
    auto moveTime = node.Motion.MovePosnDurationMsec(moveCounts);
    BOOST_LOG_TRIVIAL(debug) << "Estimated time: " << moveTime;
    double timeout = mgr->TimeStampMsec() + moveTime + 100;         //define a timeout in case the node is unable to enable

    while (!node.Motion.MoveIsDone()) {
        if (mgr->TimeStampMsec() > timeout) {
            BOOST_LOG_TRIVIAL(error) << "Timed out waiting for move to complete";
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Node Move Done";
}


int main(int argc, char* argv[]) {

    BOOST_LOG_TRIVIAL(info) << "Starting Teknic motor interface. Enumerating devices:";
    MotorAPI mapi;
    //At this point we will execute 5 rev moves sequentially on each axis
    for (size_t i = 0; i < 5; i++) {
        for (size_t nodeIndex = 0; nodeIndex < mapi.m_nodeCount; nodeIndex++) {
            mapi.moveNode(mapi.m_nodes[nodeIndex].get());
        } // for each node
    } // for each move
    return 0;
}

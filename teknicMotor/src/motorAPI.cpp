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
#include <chrono>
#include <thread>
#include "motorAPI.h"


using namespace sFnd;

//! These defines only work for the one axis. Need to define these PER AXIS
#define HOMING_TIMEOUT		    10000	//The timeout used for homing (ms)
#define CONVERSION_ERROR  -1

// defines the length of the linear rail for first axis, 24cm
#define MIN_POSITION 0.1  // in mm
#define MAX_POSITION 240
#define MAX_DISTANCE_CNTS		-105000	// --->toward chair direction, assume 0 is home

/* Convert from mm of travel to encoder counts. Must be established by measuring travel */
long MotorAPI::convertPositionToCount(long posInMM) {
	if ((posInMM < MIN_POSITION) || (posInMM > MAX_POSITION)) return CONVERSION_ERROR;
	return posInMM * MAX_DISTANCE_CNTS / MAX_POSITION;
}


// Arbitrary 1-10 scaling factor.
#define MIN_ACC_LEVEL   1
#define MAX_ACC_LEVEL   10
#define MAX_ACC_LIM_RPM	4000

/* Convert 1-10 to servo's accel limit in RPM per sec */
long MotorAPI::convertSpeedLevelToRPM(long level) {
	if ((level < MIN_ACC_LEVEL) || (level > MAX_ACC_LEVEL)) return CONVERSION_ERROR;
	return level * MAX_ACC_LIM_RPM / MAX_ACC_LEVEL;
}


#define MIN_SPEED_LEVEL  1
#define MAX_SPEED_LEVEL  10
#define MAX_VEL_LIM_RPM	 700

/* Convert 1-10 to servo's velocity limit in RPM per sec */
long MotorAPI::convertAccLevelToRPMperSecs(long level) {
	if ((level < MIN_SPEED_LEVEL) || (level > MAX_SPEED_LEVEL)) return CONVERSION_ERROR;
	return level * MAX_VEL_LIM_RPM / MAX_SPEED_LEVEL;
}


// The timeout used for homing and move operations (in ms)
double MotorAPI::getTimeout(){ return mgr->TimeStampMsec() + HOMING_TIMEOUT; }


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
        BOOST_LOG_TRIVIAL(warning) << "Homing never setup through ClearView. The node cannot be homed: " << node.Info.UserID.Value();
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
 * Figure out how many SC4-HUBs are daisy chained (3 per port), and how many
 * servo motors (nodes) are plugged into them. Register them to the SysManager.
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
        //? I only expect one port currently, but this is for safety.
        size_t nodesOnThisPort;
        BOOST_LOG_TRIVIAL(debug) << "Iterating through " << m_portCount << " nodes.";
        for (size_t i = 0; i < m_portCount; i++) { // For each port:
            BOOST_LOG_TRIVIAL(debug) << "Iterating nodes on port " << i << ".";
            IPort &myPort = m_ports[i].get();

            // Turn off all motors when we initialize the interfaces.
            myPort.BrakeControl.BrakeSetting(0, BRAKE_ALLOW_MOTION);
            myPort.BrakeControl.BrakeSetting(1, BRAKE_ALLOW_MOTION);

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
        // (defined by the mnErr class)
        BOOST_LOG_TRIVIAL(error) << "MotorAPI() constructor | addr: " << theErr.TheAddr << " | err: " << theErr.ErrorCode << " | msg: " << theErr.ErrorMsg;
    }
}

MotorAPI::~MotorAPI(void){
    BOOST_LOG_TRIVIAL(debug) << "Teknic Shutting down. Disabling nodes, and closing port";
    for (size_t i = 0; i < m_portCount; i++) {
            IPort &myPort = mgr->Ports(i);
        for (size_t nodeIndex = 0; nodeIndex < m_nodeCount; nodeIndex++) {
            myPort.Nodes(nodeIndex).EnableReq(false);
        }
    }
    mgr->PortsClose();
    BOOST_LOG_TRIVIAL(debug) << "Teknic Shutdown!";
}


/* Generic move function built off examples. */
void MotorAPI::move(
    INode &node,
    const int &moveCounts = 1000,
    const int &speed = MAX_VEL_LIM_RPM,
    const int &accel = MAX_ACC_LIM_RPM
) {
    node.Motion.MoveWentDone();        // Clear "move done" register
    
    node.VelUnit(INode::RPM);
    node.Motion.VelLimit = speed;

    node.AccUnit(INode::RPM_PER_SEC);
    node.Motion.AccLimit = accel;

    try {
        node.Motion.MovePosnStart(moveCounts);
        BOOST_LOG_TRIVIAL(info) << "Moving Node " << node.Info.UserID.Value() << " moveCounts " << moveCounts;
        
        auto moveTime = node.Motion.MovePosnDurationMsec(moveCounts, true);
        BOOST_LOG_TRIVIAL(debug) << "Estimated move duration (abs): " << moveTime << "ms";
        
        double timeout = getTimeout() + moveTime;
        while (!node.Motion.MoveIsDone()) {
            // wait here for move
            if (mgr->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Timed out waiting for move to complete";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Move Done for " << node.Info.UserID.Value();
    } catch (mnErr& theErr) {
        // (defined by the mnErr class)
        // sFnd::_mnErr::ErrorMsg
        BOOST_LOG_TRIVIAL(error) << "moveNode() | addr: " << theErr.TheAddr << " | err: " << theErr.ErrorCode << " | msg: " << theErr.ErrorMsg;
    }
}


/* High level move function built to convert from human units to machine. */
void MotorAPI::moveNode(
    INode &node,
    const int &position,
    const int &speed = MAX_VEL_LIM_RPM,
    const int &accel = MAX_ACC_LIM_RPM
) {

}




int main(int argc, char* argv[]) {

    BOOST_LOG_TRIVIAL(info) << "Starting Teknic motor interface. Enumerating devices:";
    MotorAPI mapi;
    int moveCounts = 1000;

    //At this point we will execute 5 rev moves sequentially on each axis
    for (size_t i = 0; i < 5; i++) {
        for (size_t nodeIndex = 0; nodeIndex < mapi.m_nodeCount; nodeIndex++) {
            mapi.moveNode(mapi.m_nodes[nodeIndex].get(), moveCounts);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } // for each node
    } // for each move
    return 0;
}

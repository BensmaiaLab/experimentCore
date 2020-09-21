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
                Node wrappedNode = Node(thisPort.Nodes(nodeIndex), this);

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

double MotorAPI::TimeStampMsec() {return m_manager->TimeStampMsec();}


/* Wrapper for interface
*/
Node::Node(sFnd::INode &node, MotorAPI *mapi):
    m_node(node), m_api(mapi) {

    std::string m_name = m_node.Info.UserID.Value();
    //sFnd::SysManager *m_manager = mapi;

    // Following 3 are optional if I wish to load a config file:
    // thisNode.EnableReq(false); // Should disable Node before loading config
    // m_manager->Delay(200);     //? sleep (ms?) to make sure disable is registered?
    // theNode.Setup.ConfigLoad("Config File path");
    printDetails();
    enable();
    //TODO: Home all our nodes!
    //! home();
}

Node::~Node(void) {
    m_node.EnableReq(false);
}



/* The following statements will attempt to enable the node. First, any
shutdowns or NodeStops are cleared, finally the node is enabled */

void Node::enable() {
    m_node.Status.AlertsClear();
    m_node.Motion.NodeStopClear();
    m_node.EnableReq(true);

    //define a timeout in case the node is unable to enable
    double timeout = m_api->getTimeout();
    //This will loop checking on the Real time values of the node's Ready status
    while (!m_node.Motion.IsReady()) {
        if (m_api->TimeStampMsec() > timeout) {
            BOOST_LOG_TRIVIAL(error) << "Error: Timed out waiting for Node " << m_name << " to enable";
            return;
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Node enabled: " << m_name;
}


void Node::disable() {
    m_node.EnableReq(false);
}


/* Find home position of the node. */
void Node::home(){
    if (m_node.Motion.Homing.HomingValid()) {
        if (m_node.Motion.Homing.WasHomed()) {
            BOOST_LOG_TRIVIAL(debug) << "Node has already been homed, current position is: \t" << m_node.Motion.PosnMeasured.Value();
        } else {
            BOOST_LOG_TRIVIAL(debug) << "Node has not been homed.";
        }
        BOOST_LOG_TRIVIAL(info) << "Homing Node now...";
        m_node.Motion.Homing.Initiate();

        double timeout = m_api->getTimeout();    //define a timeout in case the node is unable to enable
        while (!m_node.Motion.Homing.WasHomed()) {
            if (m_api->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Node completed homing.";
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Homing never setup through ClearView. Node " << m_name << " cannot be homed.";
    }
}


/* Diagnostics print. */
void Node::printDetails() {
    std::string nType = "CLEARPATH_SC";
    if ( m_node.Info.NodeType() == 3) nType = "CLEARPATH_SC_ADV";
    
    BOOST_LOG_TRIVIAL(info) << "  NodeType: " <<  nType;
    BOOST_LOG_TRIVIAL(info) << "     Model: " <<  m_node.Info.Model.Value();
    BOOST_LOG_TRIVIAL(info) << "  Serial #: " <<  std::to_string(m_node.Info.SerialNumber.Value()).c_str();
    BOOST_LOG_TRIVIAL(info) << "FW version: " <<  m_node.Info.FirmwareVersion.Value();
    BOOST_LOG_TRIVIAL(info) << "    userID: " <<  m_name;
}




//! These defines only work for the one axis. Need to define these PER AXIS
#define CONVERSION_ERROR  -1

// defines the length of the linear rail for first axis, 24cm
#define MIN_POSITION  0.1  // in mm
#define MAX_POSITION  240
#define MAX_DISTANCE_CNTS -105000	// --->toward chair direction, assume 0 is home

/* Convert from mm of travel to encoder counts. Must be established by measuring travel */
long Node::convertPositionToCount(long posInMM) {
	if ((posInMM < MIN_POSITION) || (posInMM > MAX_POSITION)) return CONVERSION_ERROR;
	return posInMM * MAX_DISTANCE_CNTS / MAX_POSITION;
}


// Arbitrary 1-10 scaling factor.
#define MIN_ACC_LEVEL   1
#define MAX_ACC_LEVEL   10
#define MAX_ACC_LIM_RPM	4000

/* Convert 1-10 to servo's accel limit in RPM per sec */
long Node::convertVelToRPM(long level) {
	if ((level < MIN_ACC_LEVEL) || (level > MAX_ACC_LEVEL)) return CONVERSION_ERROR;
	return level * MAX_ACC_LIM_RPM / MAX_ACC_LEVEL;
}


#define MIN_SPEED_LEVEL  1
#define MAX_SPEED_LEVEL  10
#define MAX_VEL_LIM_RPM	 700

/* Convert 1-10 to servo's velocity limit in RPM per sec */
long Node::convertAccToRPM(long level) {
	if ((level < MIN_SPEED_LEVEL) || (level > MAX_SPEED_LEVEL)) return CONVERSION_ERROR;
	return level * MAX_VEL_LIM_RPM / MAX_SPEED_LEVEL;
}


void Node::handleAlerts() {
    // Buffer for possible messages.
    char alertList[256];

    m_node.Status.RT.Refresh();
    m_node.Status.Alerts.Refresh();

    // if an alert is present:
    if (!m_node.Status.RT.Value().cpm.AlertPresent) {   
    
        if (m_node.Status.Alerts.Value().isInAlert()) {
            // get a copy of the alert register bits and a text description of all bits set
            m_node.Status.Alerts.Value().StateStr(alertList, 256);
            BOOST_LOG_TRIVIAL(warning) << "Alerts found on this node: " << alertList;
        }
    }

    //Check to see if the node experienced torque saturation
    if (m_node.Status.HadTorqueSaturation()) {
        BOOST_LOG_TRIVIAL(warning) << "Node has experienced torque saturation since last checking\n";
    }
}



/* Generic move function built off examples. */
void Node::move(
    const int &moveCounts = 1000,
    const int &speed = MAX_VEL_LIM_RPM,
    const int &accel = MAX_ACC_LIM_RPM
) {
    // Need to do some pre-checks to make sure node is ready:
    handleAlerts();

    // Then set the velocity/accel:
    m_node.VelUnit(sFnd::INode::RPM);
    m_node.Motion.VelLimit = speed;

    m_node.AccUnit(sFnd::INode::RPM_PER_SEC);
    m_node.Motion.AccLimit = accel;

    // Now move.
    BOOST_LOG_TRIVIAL(info) << "Moving Node " << m_name << " moveCounts " << moveCounts;
    try {
        m_node.Motion.MovePosnStart(moveCounts);
        
        auto moveTime = m_node.Motion.MovePosnDurationMsec(moveCounts, true);
        BOOST_LOG_TRIVIAL(debug) << "Estimated move duration (abs): " << moveTime << "ms";
        
        double timeout = m_api->getTimeout() + moveTime;
        while (!m_node.Motion.MoveIsDone()) {
            // wait here for move
            if (m_api->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Timed out waiting for move to complete";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Move complete on " << m_name;
        //! Clear the register only if it's successful?
        // m_node.Motion.MoveWentDone();        // Clear "move done" register
    } catch (sFnd::mnErr& theErr) {
        // (defined by the mnErr class)
        // sFnd::_mnErr::ErrorMsg
        BOOST_LOG_TRIVIAL(error) << "moveNode() [" << theErr.TheAddr << "] " << theErr.ErrorCode << "| " << theErr.ErrorMsg;
        // Some test cases to see if I can do inline remediation based on code:
        if (theErr.ErrorCode == 0x80040002) { BOOST_LOG_TRIVIAL(info) << "Got timeout."; }
        if (theErr.ErrorCode == 0x8004010c) { BOOST_LOG_TRIVIAL(info) << "Move blocked."; }
    }
}


/* High level move function built to convert from human units to machine. */
void Node::moveHigh(
    const int &position,  // in mm
    const int &velLevel,
    const int &accLevel
) {
    move(
        convertPositionToCount(position),
        convertVelToRPM(velLevel),
        convertAccToRPM(accLevel));
}


#include <boost/log/trivial.hpp>
#include "node.h"

/* Requires I know:
    motorAPI
    reference to SystemManager INode
*/
Node::Node(INode &node) {
    INode & m_node = node;

    // Following 3 are optional if I wish to load a config file:
    // thisNode.EnableReq(false); // Should disable Node before loading config
    // m_manager->Delay(200);     //? sleep (ms?) to make sure disable is registered?
    // theNode.Setup.ConfigLoad("Config File path");
    printDetails();
    enable();
    home();
}

Node::~Node(void) {
    m_node.EnableReq(false);
}



/* The following statements will attempt to enable the node. First, any
shutdowns or NodeStops are cleared, finally the node is enabled */
//! I don't like that this synchronously locks on that while loop
void Node::enable() {
    m_node.Status.AlertsClear();
    m_node.Motion.NodeStopClear();
    m_node.EnableReq(true);

    //define a timeout in case the node is unable to enable
    double timeout = m_manager->getTimeout();
    //This will loop checking on the Real time values of the node's Ready status
    while (!m_node.Motion.IsReady()) {
        if (m_manager->TimeStampMsec() > timeout) {
            BOOST_LOG_TRIVIAL(error) << "Error: Timed out waiting for Node " << m_node.Info.UserID.Value() << " to enable";
            return;
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Node enabled: " << m_node.Info.UserID.Value();
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

        double timeout = getTimeout();    //define a timeout in case the node is unable to enable
        while (!m_node.Motion.Homing.WasHomed()) {
            if (m_manager->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Node completed homing.";
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Homing never setup through ClearView. The node cannot be homed: " << m_node.Info.UserID.Value();
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
    BOOST_LOG_TRIVIAL(info) << "    userID: " <<  m_node.Info.UserID.Value();
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


/* Generic move function built off examples. */
void Node::move(
    const int &moveCounts = 1000,
    const int &speed = MAX_VEL_LIM_RPM,
    const int &accel = MAX_ACC_LIM_RPM
) {
    m_node.Motion.MoveWentDone();        // Clear "move done" register
    
    m_node.VelUnit(INode::RPM);
    m_node.Motion.VelLimit = speed;

    m_node.AccUnit(INode::RPM_PER_SEC);
    m_node.Motion.AccLimit = accel;

    try {
        m_node.Motion.MovePosnStart(moveCounts);
        BOOST_LOG_TRIVIAL(info) << "Moving Node " << m_node.Info.UserID.Value() << " moveCounts " << moveCounts;
        
        auto moveTime = m_node.Motion.MovePosnDurationMsec(moveCounts, true);
        BOOST_LOG_TRIVIAL(debug) << "Estimated move duration (abs): " << moveTime << "ms";
        
        double timeout = getTimeout() + moveTime;
        while (!m_node.Motion.MoveIsDone()) {
            // wait here for move
            if (m_manager->TimeStampMsec() > timeout) {
                BOOST_LOG_TRIVIAL(error) << "Timed out waiting for move to complete";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Move Done for " << m_node.Info.UserID.Value();
    } catch (mnErr& theErr) {
        // (defined by the mnErr class)
        // sFnd::_mnErr::ErrorMsg
        BOOST_LOG_TRIVIAL(error) << "moveNode() | addr: " << theErr.TheAddr << " | err: " << theErr.ErrorCode << " | msg: " << theErr.ErrorMsg;
    }
}


/* High level move function built to convert from human units to machine. */
void Node::moveHigh(
    const int &position,  // in mm
    const int &velLevel = 10,
    const int &accLevel = 10
) {
    move(
        convertPositionToCount(position),
        convertVelToRPM(velLevel),
        convertAccToRPM(accLevel));
}


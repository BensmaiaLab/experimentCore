//Required include files
#include <stdio.h>
#include <string>
#include <iostream>
#include "pubSysCls.h"

using namespace sFnd;

//*********************************************************************************
// Converting example code into an RPC library.
// TODO: Finish turning into a self-contained class
// TODO: connect this up to gRPC.
//*********************************************************************************


#define MOVE_DISTANCE_CNTS  10000
#define TIME_TILL_TIMEOUT   10000   //The timeout used for homing (in ms)

/* The following statements will attempt to enable the node. First, any
shutdowns or NodeStops are cleared, finally the node is enabled */
//! I don't like that this synchronously locks on that while loop
int EnableNode(INode &node) {
    node.Status.AlertsClear();
    node.Motion.NodeStopClear();
    node.EnableReq(true);
    //TODO: log("Node \t%zi enabled\n", iNode);
    //define a timeout in case the node is unable to enable
    double timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;
    //This will loop checking on the Real time values of the node's Ready status
    while (!node.Motion.IsReady()) {
        if (myMgr.TimeStampMsec() > timeout) {
            //TODO: log("Error: Timed out waiting for Node %d to enable\n", iNode);
            return -1;
        }
    }
    return 0;
}


/* Find home position of the node. */
int homeNode(INode node&){
    if (node.Motion.Homing.HomingValid()) {
        if (node.Motion.Homing.WasHomed()) {
            printf("Node has already been homed, current position is: \t%8.0f \n", node.Motion.PosnMeasured.Value());
        } else {
            printf("Node has not been homed.");
        }
        printf("Homing Node now...\n");
        node.Motion.Homing.Initiate();

        timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;    //define a timeout in case the node is unable to enable
        while (!node.Motion.Homing.WasHomed()) {
            if (myMgr.TimeStampMsec() > timeout) {
                printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
                return -2;
            }
        }
        printf("Node completed homing.\n");
    } else {
        printf("Node[%d] has not had homing setup through ClearView. The node will not be homed.\n", iNode);
    }
}


/* Figure out how many SC4-HUBs are daisy chained, and how many motors are
plugged into them. Register them to the SysManager. Returns number of available ports. */
//? seems to associate COM ports with SC HUB ports
// COM portnum (as seen in device manager)
int getNumPorts(SysManager &myMgr){
    size_t portCount = 0;
    std::vector<std::string> comHubPorts;
    SysManager::FindComHubPorts(comHubPorts);

    //TODO: log.info("Found %d SC Hubs\n", comHubPorts.size());
    for (
        portCount = 0;
        portCount < comHubPorts.size() && portCount < NET_CONTROLLER_MAX;
        portCount++) {
        myMgr.ComHubPort(portCount, comHubPorts[portCount].c_str());
    }
    if (portCount < 0) {
        //TODO: log.error("Unable to locate SC hub port\n");
        return -1;
    }
    return portCount;
}

/* Diagnostics print. */
//TODO: What other deets can I print?
int printNodeDetails(INode node&) {
    //TODO: Switch this to log.info()
    printf("  NodeType: %d\n", node.Info.NodeType());
    printf("     Model: %s\n", node.Info.Model.Value());
    printf("  Serial #: %d\n", node.Info.SerialNumber.Value());
    printf("FW version: %s\n", node.Info.FirmwareVersion.Value());
    printf("    userID: %s\n", node.Info.UserID.Value());
}


/* Try to open SC4-HUB(s) over USB and query for all motors connected to it.
Can Load config file for each motor. Will enable and home all nodes. */
int initialize(SysManager myMgr&){
    try {
        size_t portCount = getNumPorts(myMgr); //! I shouldn't call this twice.

        //printf("\n I will now open port \t%i \n \n", portnum);
        myMgr.PortsOpen(portCount);

        // Iterate through ports...
        for (size_t i = 0; i < portCount; i++) {
            IPort &myPort = myMgr.Ports(i);
            printf(" Port[%d]: state=%d, nodes=%d\n", myPort.NetNumber(), myPort.OpenState(), myPort.NodeCount());

            // Iterate through nodes available on the port
            for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
                INode &theNode = myPort.Nodes(iNode); // Create a shortcut reference for a node

                theNode.EnableReq(false); // Should disable Node before loading config
                myMgr.Delay(200); //? sleep (ms?) to make sure disable is registered
                //theNode.Setup.ConfigLoad("Config File path");
                printNodeDetails(theNode);
                enableNode(theNode);
                homeNode(theNode);

            }
        }
    }  catch (mnErr& theErr) {
        //This statement will print the address of the error, the error code
        // (defined by the mnErr class), as well as the corresponding error message.
        printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
    }
}

int shutdown(SysManager myMgr&) {
    printf("Disabling nodes, and closing port\n");
    //Disable Nodes
    size_t portCount = getNumPorts(myMgr);
    for (size_t i = 0; i < portCount; i++) {
            IPort &myPort = myMgr.Ports(i);
        for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
            myPort.Nodes(iNode).EnableReq(false);
        }
    }
    myMgr.PortsClose();
}



int main(int argc, char* argv[]) {
    SysManager myMgr;
    initialize(myMgr);

    //At this point we will execute i rev moves sequentially on each axis
    for (size_t i = 0; i < 5; i++) {
        for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
            // Create a shortcut reference for a node
            INode &theNode = myPort.Nodes(iNode);

            theNode.Motion.MoveWentDone();                      //Clear the rising edge Move done register

            theNode.AccUnit(INode::RPM_PER_SEC);                //Set the units for Acceleration to RPM/SEC
            theNode.VelUnit(INode::RPM);                        //Set the units for Velocity to RPM
            theNode.Motion.AccLimit = 100000;      // Set Acceleration Limit (RPM/Sec)
            theNode.Motion.VelLimit = 700;              //Set Velocity Limit (RPM)

            printf("Moving Node \t%zi \n", iNode);
            theNode.Motion.MovePosnStart(MOVE_DISTANCE_CNTS);           //Execute 10000 encoder count move
            printf("%f estimated time.\n", theNode.Motion.MovePosnDurationMsec(MOVE_DISTANCE_CNTS));
            double timeout = myMgr.TimeStampMsec() + theNode.Motion.MovePosnDurationMsec(MOVE_DISTANCE_CNTS) + 100;         //define a timeout in case the node is unable to enable

            while (!theNode.Motion.MoveIsDone()) {
                if (myMgr.TimeStampMsec() > timeout) {
                    printf("Error: Timed out waiting for move to complete\n");
                    msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
                    return -2;
                }
            }
            printf("Node \t%zi Move Done\n", iNode);
        } // for each node
    } // for each move

    //After moves have completed Disable node, and close ports
    shutdown(myMgr);

    // Close down the ports
    

}

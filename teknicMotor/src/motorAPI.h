#include <vector>
#include <sstream>
#include <pubSysCls.h>

using namespace sFnd;

class MotorAPI {
    private:
        sFnd::SysManager *mgr;

        long MotorAPI::convertPositionToCount(long posInMM);
        long MotorAPI::convertSpeedLevelToRPM(long level);
        long MotorAPI::convertAccLevelToRPMperSecs(long level);
        double getTimeout();

    public:
        size_t m_portCount = 0;
        std::vector<std::reference_wrapper<IPort>> m_ports;

        size_t m_nodeCount = 0;
        std::vector<std::reference_wrapper<INode>> m_nodes;

        MotorAPI();
        ~MotorAPI();

        void enableNode(INode &node);
        void homeNode(INode &node);
        void printNodeDetails(INode &node);
        void move(INode &node, const int &moveCounts, const int &speed, const int &accel);
        
        void moveNode(INode &node, const int &position, const int &speed, const int &accel);
};

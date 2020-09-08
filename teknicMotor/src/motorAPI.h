#include <vector>
#include <sstream>
#include <pubSysCls.h>

using namespace sFnd;

class MotorAPI {
    private:
        sFnd::SysManager *mgr;

    public:
        size_t m_portCount = 0;
        std::vector<std::reference_wrapper<IPort>> m_ports;

        size_t m_nodeCount = 0;
        std::vector<std::reference_wrapper<INode>> m_nodes;

        MotorAPI();
        ~MotorAPI();

        double getTimeout();
        void enableNode(INode &node);
        void homeNode(INode &node);
        void printNodeDetails(INode &node);
        void moveNode(INode &node);
};

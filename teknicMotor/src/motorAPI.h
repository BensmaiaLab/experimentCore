#include <vector>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <pubSysCls.h>
#include "node.h"

using namespace sFnd;

class MotorAPI {
private:
    sFnd::SysManager *m_manager;

public:
    size_t m_portCount = 0;
    std::vector<std::reference_wrapper<IPort>> m_ports;

    size_t m_nodeCount = 0;
    std::vector<std::reference_wrapper<Node>> m_nodes;

    MotorAPI();
    ~MotorAPI();
    double getTimeout();
    
};

#include <vector>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <pubSysCls.h>

class Node;
class MotorAPI {
private:
    sFnd::SysManager *m_manager;

public:
    size_t m_portCount = 0;
    std::vector<std::reference_wrapper<sFnd::IPort>> m_ports;

    size_t m_nodeCount = 0;
    std::vector<std::reference_wrapper<Node>> m_nodes;

    MotorAPI();
    ~MotorAPI();
    double getTimeout();
    double TimeStampMsec();
    
};

// Was considering direct inheritance, but no?
// class Node: public sFnd::INode

class Node {
private:
    sFnd::INode &m_node;
    MotorAPI *m_api;
    Node() = delete;
    std::string m_name;

    long convertPositionToCount(long posInMM);
    long convertVelToRPM(long level);
    long convertAccToRPM(long level);

public:
    Node(sFnd::INode &node, MotorAPI *mapi);
    ~Node(void);
    void move(const int &moveCounts, const int &speed, const int &accel);
    void moveHigh(const int &position, const int &velLevel = 10, const int &accLevel = 10);
    void enable();
    void disable();
    void home();
    void printDetails();

};

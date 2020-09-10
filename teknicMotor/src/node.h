#include <boost/log/trivial.hpp>
#include <pubSysCls.h>

using namespace sFnd;

class Node : public INode {

private:
    // 
    INode &m_node;
    Node() = delete;

    // methods
    long convertPositionToCount(long posInMM);
    long convertVelToRPM(long level);
    long convertAccToRPM(long level);

    void enable();
    void disable();
    void home();
    void printDetails();

public:
    Node(INode &node);
    ~Node(void);
    void move(const int &moveCounts, const int &speed, const int &accel);
    void moveHigh(const int &position, const int &velLevel, const int &accLevel);


}
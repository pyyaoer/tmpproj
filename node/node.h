#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule {
protected:
    int counter;
    cMessage *event;
    cMessage *tictocMsg;
    cMessage *newTaskEvent;
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
public:
    Node();
    virtual ~Node();
};

Define_Module(Node);
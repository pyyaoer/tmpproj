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

class IoT : public Node {
    cPar *sleepTime;
    cPar *burstTime;
    cPar *sendIATime;

    cMessage *startStopBurst;
    cMessage *sendMessage;

    // state
    cFSM fsm;
    enum {
        INIT = 0,
        SLEEP = FSM_Steady(1),
        ACTIVE = FSM_Steady(2),
        SEND = FSM_Transient(1),
    };

protected:
    int tenant_id;

    virtual void handleMessage(cMessage *msg) override;
    virtual void processTimer(cMessage *msg);
    void processMessage(cMessage *msg);

    void generateTask();

    virtual ~IoT();
    void initialize() override;
};

Define_Module(IoT);

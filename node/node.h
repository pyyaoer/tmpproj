#include <string.h>
#include <omnetpp.h>

#include "NodeMessage_m.h"

using namespace omnetpp;

#define TENANT_NUM 8

class Node : public cSimpleModule {
protected:
    int id;
    virtual void initialize() override {};
    virtual void handleMessage(cMessage *msg) override {};
public:
    Node() {}
    virtual ~Node() {};
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

public:
    IoT();
    virtual ~IoT();
    void initialize() override;
};

Define_Module(IoT);

class Edge : public Node {
    double delta[TENANT_NUM];
    double rho[TENANT_NUM];
    cPar *syncTime;
    cMessage *syncMessage;

    // state
    cFSM fsm;
    enum {
        INIT = 0,
        SLEEP = FSM_Steady(1),
        SYNC = FSM_Transient(1),
    };

protected:

    virtual void handleMessage(cMessage *msg) override;
    virtual void processTimer(cMessage *msg);
    void processMessage(TaskMessage *msg);

    void sync();

public:
    Edge();
    virtual ~Edge();
    void initialize() override;
};

Define_Module(Edge);

class PNode : public Node {

protected:

    virtual void handleMessage(cMessage *msg) override;
    void processMessage(cMessage *msg);

public:
    PNode();
    virtual ~PNode();
    void initialize() override;
};

Define_Module(PNode);

class SubPNode : public Node {

protected:

    virtual void handleMessage(cMessage *msg) override;
    void processMessage(cMessage *msg);

public:
    SubPNode();
    virtual ~SubPNode();
    void initialize() override;
};

Define_Module(SubPNode);

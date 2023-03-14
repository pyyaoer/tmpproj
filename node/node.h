#include <string.h>

#include "util.h"
#include "NodeMessage_m.h"

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
    void processMessage(BaseMessage *msg);

    void generateTask();

public:
    IoT();
    virtual ~IoT();
    void initialize() override;
};

Define_Module(IoT);

class Edge : public Node {
    friend class Executor;

    double delta[TENANT_NUM];
    double rho[TENANT_NUM];
    double r_req[TENANT_NUM];
    double l_req[TENANT_NUM];
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
    void processMessage(BaseMessage *msg);

    void sync();
    int scan();

public:
    Edge();
    virtual ~Edge();
    void initialize() override;
};

Define_Module(Edge);

class PNodeBase : public Node {

protected:
    std::vector<Records> record_r_;
    std::vector<Records> record_l_;

    void sync(SyncMessage *smsg);

public:
    PNodeBase();
    virtual ~PNodeBase();
};

Define_Module(PNodeBase);

class SubPNode : public PNodeBase {

protected:

    virtual void handleMessage(cMessage *msg) override;
    void processMessage(BaseMessage *msg);

public:
    SubPNode();
    virtual ~SubPNode();
    void initialize() override;
};

Define_Module(SubPNode);

class PNode : public PNodeBase {

protected:
    virtual void handleMessage(cMessage *msg) override;
    void processMessage(BaseMessage *msg);

public:
    PNode();
    virtual ~PNode();
    void initialize() override;
};

Define_Module(PNode);

class Executor : public cSimpleModule {

    cMessage *doneMessage;
    cMessage *scanMessage;
    cMessage *waitMessage;

    simtime_t scan_interval;
    simtime_t duration;

    cFSM fsm;
    enum {
        INIT = 0,
        WAITING = FSM_Steady(1),
        RUNNING = FSM_Steady(2),
        FINDING = FSM_Transient(1),
    };
protected:
    virtual void handleMessage(cMessage *msg) override;

public:
    Executor() {};
    ~Executor() {
        delete doneMessage;
        delete scanMessage;
        delete waitMessage;
    };
    void initialize() override;
};

Define_Module(Executor);

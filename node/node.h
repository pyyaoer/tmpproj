#include <string.h>
#include <queue>

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
    double duration;

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
    typedef struct {
        int task_id;
        int tenant_id;
        double duration;
        simtime_t creation;
        int gate;
    } Task;

    cPar *syncTime;
    cMessage *syncMessage;
    int exe_n;

    // state
    cFSM fsm;
    enum {
        INIT = 0,
        SLEEP = FSM_Steady(1),
        SYNC = FSM_Transient(1),
    };

    int task_counter;
    std::queue<Task> todo_;
    std::vector<Task> doing_;

protected:

    virtual void handleMessage(cMessage *msg) override;
    virtual void processTimer(cMessage *msg);
    void processMessage(BaseMessage *msg);

    void sync();
    void scan(int executor_id);
    void done(int task_id);

public:
    Edge();
    virtual ~Edge();
    void initialize() override;
};

Define_Module(Edge);

class PNodeBase : public Node {

protected:

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

    int executor_id;
    int task_id;

    cMessage *doneMessage;
    cMessage *scanMessage;

    simtime_t scan_interval;
    simtime_t duration;

    cFSM fsm;
    enum {
        INIT = 0,
        WAITING = FSM_Steady(1),
        RUNNING = FSM_Steady(2),
        FINDING = FSM_Steady(3),
    };
protected:
    virtual void handleMessage(cMessage *msg) override;

public:
    Executor();
    ~Executor();
    void initialize() override;
};

Define_Module(Executor);

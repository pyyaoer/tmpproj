#include <string.h>
#include <queue>
#include <exception>

#include "util.h"
#include "NodeMessage_m.h"


class m2 : public cSimpleModule {
protected:
    std::vector<Latency> latency;
    std::vector<cOutVector> lat_vec;
    std::vector<cHistogram *> lat_hist;
    std::vector<std::queue<simtime_t>> iops_window;
    std::vector<cOutVector> iops_vec;
    double window_size;
public:
    m2() : latency(TENANT_NUM), lat_vec(TENANT_NUM), lat_hist(TENANT_NUM),
            iops_window(TENANT_NUM), iops_vec(TENANT_NUM) {}
    virtual void initialize() override;
    void update_latency(int tenant_id, double latency_data, double arrival_time);
};

Define_Module(m2);

class Node : public cSimpleModule {
protected:
    int id;
    std::vector<double> r;
    std::vector<double> l;
    virtual void initialize() override {
        id = par("id").intValue();
        r = cStringTokenizer(par("r").stdstringValue().c_str()).asDoubleVector();
        l = cStringTokenizer(par("l").stdstringValue().c_str()).asDoubleVector();
    };
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

    simtime_t r_last[TENANT_NUM];
    simtime_t l_last[TENANT_NUM];

    double delta[TENANT_NUM];
    double rho[TENANT_NUM];
    double r_req[TENANT_NUM];
    double l_req[TENANT_NUM];

    std::vector<VirtualTime> r_timeline;
    std::vector<VirtualTime> l_timeline;

    int exe_n;
    double sync_period;
    cMessage *syncMessage;

    // state
    cFSM fsm;
    enum {
        INIT = 0,
        SLEEP = FSM_Steady(1),
        SYNC = FSM_Transient(1),
    };

    int task_counter;
    int last_scheduled_tenant;
    std::vector<std::vector<Task>> todo_;
    std::vector<Task> doing_;

protected:

    virtual void handleMessage(cMessage *msg) override;
    virtual void processTimer(cMessage *msg);
    void processMessage(BaseMessage *msg);

    void sync_p();
    void scan(int executor_id);
    void done(int task_id);
    Task schedule();
    void new_task(Task t);

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
    std::vector<Records> record_sub_r_;
    std::vector<Records> record_sub_l_;
    int tenant_n;
    double scaling_r[TENANT_NUM];
    double scaling_l[TENANT_NUM];

    void sync_edge(SyncMessage *smsg);

public:
    PNodeBase();
    virtual ~PNodeBase();
    void initialize() override;
};

Define_Module(PNodeBase);

class SubPNode : public PNodeBase {

    double sync_period;
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

    void sync_p();

public:
    SubPNode();
    virtual ~SubPNode();
    void initialize() override;
};

Define_Module(SubPNode);

class PNode : public PNodeBase {

    int subp_n;

protected:
    virtual void handleMessage(cMessage *msg) override;
    void processMessage(BaseMessage *msg);

    void sync_subp(SubpSyncMessage *smsg);

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

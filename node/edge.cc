//#define FSM_DEBUG
#include "node.h"
#include <stdio.h>

Edge::Edge() : Node(), todo_(TENANT_NUM), r_timeline(TENANT_NUM),
               l_timeline(TENANT_NUM) {
    task_counter = 0;
    syncMessage = nullptr;
}

Edge::~Edge() {
    cancelAndDelete(syncMessage);
}

void Edge::initialize() {
    Node::initialize();
    fsm.setName("fsm");
    fsm.setState(INIT);

    sync_period = par("sync_period").doubleValue();
    syncMessage = new cMessage("syncMessage");

    exe_n = par("exe_n").intValue();
    cModuleType *moduleType = cModuleType::get("node.Executor");
    for (int i = 0; i < exe_n; ++i) {
        char name[20] = {0};
        sprintf(name, "executor%d", i);
        cModule *executor = moduleType->createScheduleInit(name, this);
        this->gate("executor_port$o", i)->connectTo(executor->gate("host_port$i"));
        executor->gate("host_port$o")->connectTo(this->gate("executor_port$i", i));
        ExeActMessage *msg = new ExeActMessage();
        msg->setType(EXE_ACT_MESSAGE);
        msg->setExecutor_id(i);
        send(msg, "executor_port$o", i);
    }

    simtime_t now = simTime();
    for (int i = 0; i < TENANT_NUM; ++i) {
        r_last[i] = l_last[i] = now;
        r_req[i] = l_req[i] = 0;
        rho[i] = delta[i] = 1;
    }

    scheduleAt(0, syncMessage);
}

void Edge::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(check_and_cast<BaseMessage *>(msg));
}

void Edge::processTimer(cMessage *msg) {
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            FSM_Goto(fsm, SLEEP);
            break;

        case FSM_Enter(SLEEP):
            scheduleAt(simTime() + sync_period, syncMessage);
            break;

        case FSM_Exit(SLEEP):
            FSM_Goto(fsm, SYNC);
            break;

        case FSM_Exit(SYNC): {
            sync_p();
            FSM_Goto(fsm, SLEEP);
            break;
        }
    };
}

void Edge::processMessage(BaseMessage *msg) {
    TaskMessage* tmsg;
    InfoMessage* imsg;
    ExeScanMessage* etmsg;
    ExeDoneMessage* edmsg;
    int i;
    switch (msg->getType()) {
        case TASK_MESSAGE:
            tmsg = check_and_cast<TaskMessage *>(msg);
            new_task(Task({task_counter++,
                            tmsg->getTenant_id(),
                            tmsg->getTask_duration(),
                            tmsg->getCreationTime(),
                            tmsg->getArrivalGate()->getIndex()}));
            break;
        case INFO_MESSAGE:
            imsg = check_and_cast<InfoMessage *>(msg);
            for (i = 0; i < TENANT_NUM; i++) {
                rho[i] = imsg->getRho(i);
                delta[i] = imsg->getDelta(i);
            }
            break;
        case EXE_SCAN_MESSAGE:
            etmsg = check_and_cast<ExeScanMessage *>(msg);
            scan(etmsg->getExecutor_id());
            break;
        case EXE_DONE_MESSAGE:
            edmsg = check_and_cast<ExeDoneMessage *>(msg);
            done(edmsg->getTask_id());
            break;
        default:
            EV << "unexpected message type " << msg->getType() << " in Edge\n";
            break;
    }
    delete msg;
}

void Edge::sync_p() {
    SyncMessage *msg = new SyncMessage();
    msg->setType(SYNC_MESSAGE);
    msg->setEdge_id(id);
    msg->setPeriod(sync_period);
    for (int i=0; i<TENANT_NUM; ++i) {
        msg->setR(i, r_req[i]);
        msg->setL(i, l_req[i]);
        r_req[i] = l_req[i] = 0;
    }
    send(msg, "pnode_port$o");
}

void Edge::scan(int executor_id) {
    ExeTaskMessage *msg = new ExeTaskMessage();
    msg->setType(EXE_TASK_MESSAGE);
    msg->setSucc(0);
    Task t = schedule();
    if (t.task_id >= 0) {
        doing_.push_back(t);
        msg->setSucc(1);
        msg->setTask_id(t.task_id);
        msg->setDuration(t.duration);
    }
    send(msg, "executor_port$o", executor_id);
}

Task Edge::schedule() {
    TaskTime tt({-1,-1,simTime()});
    bool succ = false;
    for (int i = 1; i <= TENANT_NUM; ++i) {
        int idx = (i + last_scheduled_tenant) % TENANT_NUM;
        if (r_timeline[idx].has_ready()) {
            tt = r_timeline[idx].pop_item();
            l_timeline[idx].remove_item(tt.task_id);
            r_req[tt.tenant_id]++;
            l_req[tt.tenant_id]++;
            last_scheduled_tenant = idx;
            succ = true;
            break;
        }
    }
    if (!succ) {
        for (int i = 1; i <= TENANT_NUM; ++i) {
            int idx = (i + last_scheduled_tenant) % TENANT_NUM;
            if (l_timeline[idx].has_ready()) {
                tt = l_timeline[idx].pop_item();
                r_timeline[idx].remove_item(tt.task_id);
                l_req[tt.tenant_id]++;
                last_scheduled_tenant = idx;
                succ = true;
                break;
            }
        }
    }
    Task t;
    t.task_id = -1;
    if (succ) {
        for (auto iter = todo_[tt.tenant_id].begin(); iter != todo_[tt.tenant_id].end(); iter++) {
            if (iter->task_id == tt.task_id) {
                t = *iter;
                todo_[tt.tenant_id].erase(iter);
                break;
            }
        }
    }
    return t;
}

void Edge::new_task(Task t) {
    simtime_t now = simTime();
    int tenant_id = t.tenant_id;
    todo_[tenant_id].push_back(t);
    TaskTime r_time({
        t.task_id,
        t.tenant_id,
        std::max(r_last[tenant_id]+rho[tenant_id]/r[tenant_id], now)
    });
    r_last[tenant_id] = r_time.time;
    r_timeline[tenant_id].insert_item(r_time);
    TaskTime l_time({
        t.task_id,
        t.tenant_id,
        std::max(l_last[tenant_id]+delta[tenant_id]/l[tenant_id], now)
    });
    l_last[tenant_id] = l_time.time;
    l_timeline[tenant_id].insert_item(l_time);
}

void Edge::done(int task_id) {
    Task t;
    CompMessage* cmsg = new CompMessage();
    for (auto i = doing_.begin(); i != doing_.end(); i++) {
        if (i->task_id == task_id) {
            t = *i;
            doing_.erase(i);
            break;
        }
    }
    cmsg->setType(COMP_MESSAGE);
    cmsg->setCreation(t.creation);
    send(cmsg, "iot_port$o", t.iot);
}


//#define FSM_DEBUG
#include "node.h"
#include <stdio.h>

Edge::Edge() : Node(), todo_(TENANT_NUM) {
    task_counter = 0;
    syncMessage = nullptr;
}

Edge::~Edge() {
    cancelAndDelete(syncMessage);
}

void Edge::initialize() {
    fsm.setName("fsm");
    fsm.setState(INIT);

    id = par("id").intValue();
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
    switch (msg->getType()) {
        case TASK_MESSAGE:
            tmsg = check_and_cast<TaskMessage *>(msg);
            todo_[tmsg->getTenant_id()].push(Task({task_counter++,
                                             tmsg->getTenant_id(),
                                             tmsg->getTask_duration(),
                                             tmsg->getCreationTime(),
                                             tmsg->getArrivalGate()->getIndex()}));
            break;
        case INFO_MESSAGE:
            imsg = check_and_cast<InfoMessage *>(msg);
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

Edge::Task Edge::schedule() {
    // naive approach: select the oldest one
    simtime_t first_task_t = simTime();
    int first_task_tenant = -1;
    for (int i = 0; i < TENANT_NUM; ++i) {
        if (todo_[i].empty() or todo_[i].front().creation >= first_task_t)
            continue;
        first_task_t = todo_[i].front().creation;
        first_task_tenant = i;
    }
    Task t;
    t.task_id = -1;
    if (first_task_tenant >= 0) {
        t = todo_[first_task_tenant].front();
        todo_[first_task_tenant].pop();
    }
    return t;
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
    send(cmsg, "iot_port$o", t.gate);
}


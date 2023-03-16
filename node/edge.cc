//#define FSM_DEBUG
#include "node.h"
#include <stdio.h>

Edge::Edge() : Node() {
    task_counter = 0;
    syncMessage = nullptr;
}

Edge::~Edge() {
    cancelAndDelete(syncMessage);
}

void Edge::initialize() {
    fsm.setName("fsm");
    fsm.setState(INIT);

    for (int i = 0; i < TENANT_NUM; ++i) {
        delta[i] = rho[i] = GATE_NUM;
    }

    id = par("id").intValue();
    syncTime = &par("syncTime");
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
    // process the self-message or incoming packet
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(check_and_cast<BaseMessage *>(msg));
}

void Edge::processTimer(cMessage *msg) {
    simtime_t d;
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            // transition to SLEEP state
            FSM_Goto(fsm, SLEEP);
            break;

        case FSM_Enter(SLEEP):
            // schedule end of sleep period (start of next sync)
            d = syncTime->doubleValue();
            scheduleAt(simTime() + d, syncMessage);
            break;

        case FSM_Exit(SLEEP):
            // schedule end of this burst
            EV << "sync data with PNode\n";
            FSM_Goto(fsm, SYNC);
            break;

        case FSM_Exit(SYNC): {
            // Sync with PNode
            sync();

            // return to SLEEP
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
            todo_.push(Task({task_counter++,
                             tmsg->getTenant_id(),
                             tmsg->getTask_duration(),
                             tmsg->getCreationTime(),
                             tmsg->getArrivalGate()->getIndex()}));
            break;
        case INFO_MESSAGE:
            imsg = check_and_cast<InfoMessage *>(msg);
            for (i=0; i<TENANT_NUM; ++i) {
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

void Edge::sync() {
    SyncMessage *msg = new SyncMessage();
    msg->setType(SYNC_MESSAGE);
    msg->setGate_id(id);
    msg->setPeriod(syncTime->doubleValue());
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
    if (!todo_.empty()) {
        Task t = todo_.front();
        todo_.pop();
        doing_.push_back(t);
        msg->setSucc(1);
        msg->setTask_id(t.task_id);
        msg->setDuration(t.duration);
    }
    send(msg, "executor_port$o", executor_id);
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

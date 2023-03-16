#include "node.h"

Executor::Executor() {
    doneMessage = nullptr;
    scanMessage = nullptr;
}

Executor::~Executor() {
    cancelAndDelete(doneMessage);
    cancelAndDelete(scanMessage);
}

void Executor::initialize() {
    fsm.setName("fsm");
    fsm.setState(INIT);

    doneMessage = new cMessage();
    scanMessage = new cMessage();

    scan_interval = SimTime(10, SIMTIME_MS);
}

void Executor::handleMessage(cMessage *msg) {
    ExeActMessage *eamsg;
    ExeScanMessage *esmsg;
    ExeTaskMessage *etmsg;
    ExeDoneMessage *edmsg;
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            eamsg = check_and_cast<ExeActMessage *>(msg);
            if (eamsg->getType() == EXE_ACT_MESSAGE) {
                executor_id = eamsg->getExecutor_id();
                FSM_Goto(fsm, WAITING);
            }
            else {
                throw cRuntimeError("invalid event in executor state INIT");
            }
            delete msg;
            break;

        case FSM_Enter(WAITING):
            scheduleAt(simTime() + scan_interval, scanMessage);
            break;

        case FSM_Exit(WAITING):
            FSM_Goto(fsm, FINDING);
            break;

        case FSM_Enter(RUNNING):
            break;

        case FSM_Exit(RUNNING):
            if (msg == doneMessage) {
                cancelEvent(doneMessage);
                edmsg = new ExeDoneMessage();
                edmsg->setType(EXE_DONE_MESSAGE);
                edmsg->setTask_id(task_id);
                send(edmsg, "host_port$o");
                FSM_Goto(fsm, WAITING);
            }
            else 
                throw cRuntimeError("invalid event in executor state RUNNING");
            break;

        case FSM_Enter(FINDING):
            esmsg = new ExeScanMessage();
            esmsg->setType(EXE_SCAN_MESSAGE);
            esmsg->setExecutor_id(executor_id);
            send(esmsg, "host_port$o");
            break;

        case FSM_Exit(FINDING): {
            etmsg = check_and_cast<ExeTaskMessage *>(msg);
            if (etmsg->getType() != EXE_TASK_MESSAGE)
                throw cRuntimeError("invalid event in executor state FINDING");
            if (etmsg->getSucc()) {
                task_id = etmsg->getTask_id();
                cancelEvent(scanMessage);
                scheduleAt(simTime() + etmsg->getDuration(), doneMessage);
                FSM_Goto(fsm, RUNNING);
            }
            else {
                FSM_Goto(fsm, WAITING);
            }
            delete msg;

            break;
        }
    };
}

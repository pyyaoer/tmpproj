#include "node.h"

void Executor::initialize() {
    fsm.setName("fsm");
    fsm.setState(WAITING);

    doneMessage = new cMessage();
    scanMessage = new cMessage();
    waitMessage = new cMessage();

    scan_interval = SimTime(10, SIMTIME_US);
}

void Executor::handleMessage(cMessage *msg) {
    ExeActMessage *emsg;
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            emsg = check_and_cast<ExeActMessage *>(msg);
            if (emsg->getType() == EXEACT_MESSAGE) {
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
                FSM_Goto(fsm, WAITING);
            }
            else
                throw cRuntimeError("invalid event in executor state RUNNING");
            break;

        case FSM_Exit(FINDING): {
            if (true) {
                cancelEvent(scanMessage);
                scheduleAt(simTime() + duration, doneMessage);
                FSM_Goto(fsm, RUNNING);
            }
            else {
                FSM_Goto(fsm, WAITING);
            }

            break;
        }
    };
}

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
    FSM_Switch(fsm) {
        case FSM_Exit(INIT):
            FSM_Goto(fsm, WAITING);
            break;

        case FSM_Enter(WAITING):
            scheduleAt(simTime() + scan_interval, scanMessage);
            break;

        case FSM_Exit(WAITING):
            FSM_Goto(fsm, FINDING);
            break;

        case FSM_Enter(RUNNING):
            scheduleAt(simTime() + duration, doneMessage);
            break;

        case FSM_Exit(RUNNING):
            // transition to either SEND or SLEEP
            if (msg == doneMessage) {
                cancelEvent(doneMessage);
                FSM_Goto(fsm, WAITING);
            }
            else
                throw cRuntimeError("invalid event in state RUNNING");
            break;

        case FSM_Exit(FINDING): {

            if (true) {
                cancelEvent(scanMessage);
                FSM_Goto(fsm, RUNNING);
            }
            else {
                FSM_Goto(fsm, WAITING);
            }

            break;
        }
    };
}

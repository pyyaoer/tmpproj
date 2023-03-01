#define FSM_DEBUG
#include "node.h"

Edge::Edge() {
    syncMessage = nullptr;
}

Edge::~Edge() {
    cancelAndDelete(syncMessage);
}

void Edge::initialize() {
    fsm.setName("fsm");

    syncTime = &par("syncTime");
    syncMessage = new cMessage("syncMessage");

    scheduleAt(0, syncMessage);
}

void Edge::handleMessage(cMessage *msg) {
    // process the self-message or incoming packet
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(msg);
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
            //scheduleAt(simTime() + d, syncMessage);
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
    }
}

void Edge::processMessage(cMessage *msg) {
}

void Edge::sync() {
    cMessage *msg = new cMessage();
    send(msg, "pnode_port$o");
}


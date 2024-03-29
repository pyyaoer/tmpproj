#define FSM_DEBUG
#include "node.h"

SubPNode::SubPNode() : PNodeBase() {
    syncMessage = nullptr;
}

SubPNode::~SubPNode() {
    cancelAndDelete(syncMessage);
}

void SubPNode::initialize() {
    PNodeBase::initialize();
    fsm.setName("fsm");
    fsm.setState(INIT);

    sync_period = par("sync_period").doubleValue();
    syncMessage = new cMessage("syncMessage");

    scheduleAt(0, syncMessage);
}

void SubPNode::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processMessage(check_and_cast<BaseMessage *>(msg));
}

void SubPNode::processMessage(BaseMessage *msg) {
    SubpInfoMessage *imsg;
    switch (msg->getType()) {
        case SYNC_MESSAGE:
            sync_edge(check_and_cast<SyncMessage *>(msg));
            break;
        case SUBP_INFO_MESSAGE:
            imsg = check_and_cast<SubpInfoMessage *>(msg);
            break;
        default:
            break;
    }
    delete msg;
}

void SubPNode::processTimer(cMessage *msg) {
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

void SubPNode::sync_p() {
    SubpSyncMessage *msg = new SubpSyncMessage();
    msg->setType(SUBP_SYNC_MESSAGE);
    msg->setSubp_id(id);
    msg->setPeriod(sync_period);
    send(msg, "pnode_port$o");
}

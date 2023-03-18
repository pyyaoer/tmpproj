#define FSM_DEBUG
#include "node.h"

PNode::PNode() : PNodeBase() {
}

PNode::~PNode() {
}

void PNode::initialize() {
    PNodeBase::initialize();
}

void PNode::handleMessage(cMessage *msg) {
    processMessage(check_and_cast<BaseMessage *>(msg));
}

void PNode::processMessage(BaseMessage *msg) {
    switch (msg->getType()) {
        case SYNC_MESSAGE:
            sync_edge(check_and_cast<SyncMessage *>(msg));
            break;
        case SUBP_SYNC_MESSAGE:
            sync_subp(check_and_cast<SubpSyncMessage *>(msg));
            break;
        default:
            break;
    }
    delete msg;
}

void PNode::sync_subp(SubpSyncMessage *smsg) {
    int subp_id = smsg->getSubp_id();
    SubpInfoMessage * msg = new SubpInfoMessage();
    msg->setType(SUBP_INFO_MESSAGE);
    send(msg, "subp_port$o", subp_id);
}

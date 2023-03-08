#define FSM_DEBUG
#include "node.h"

SubPNode::SubPNode() : PNodeBase() {
}

SubPNode::~SubPNode() {
}

void SubPNode::initialize() {
    id = par("id").intValue();
}

void SubPNode::handleMessage(cMessage *msg) {
    processMessage(check_and_cast<BaseMessage *>(msg));
}

void SubPNode::processMessage(BaseMessage *msg) {
    SyncMessage *smsg;
    switch (msg->getType()) {
        case SYNC_MESSAGE:
            sync(check_and_cast<SyncMessage *>(msg));
            break;
        default:
            break;
    }
    delete msg;
}

#define FSM_DEBUG
#include "node.h"

PNode::PNode() : PNodeBase() {
}

PNode::~PNode() {
}

void PNode::initialize() {
    id = par("id").intValue();
}

void PNode::handleMessage(cMessage *msg) {
    processMessage(check_and_cast<BaseMessage *>(msg));
}

void PNode::processMessage(BaseMessage *msg) {
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

#define FSM_DEBUG
#include "node.h"

PNode::PNode() {
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
            smsg = check_and_cast<SyncMessage *>(msg);
            sync(smsg->getGate_id());
            break;
        default:
            break;
    }
    delete msg;
}

void PNode::sync(int gate_id) {
    TagMessage * msg = new TagMessage();
    for (int i = 0; i < TENANT_NUM; ++i) {
        msg->setRho(i, 1);
        msg->setDelta(i, 1);
    }
    send(msg, "edge_port$o", gate_id);
}

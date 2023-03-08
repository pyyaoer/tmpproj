#define FSM_DEBUG
#include "node.h"

SubPNode::SubPNode() {
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
    delete msg;
}


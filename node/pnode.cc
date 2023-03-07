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
}


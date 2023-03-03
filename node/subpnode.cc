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
    processMessage(msg);
}

void SubPNode::processMessage(cMessage *msg) {
}


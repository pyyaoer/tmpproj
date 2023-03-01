#define FSM_DEBUG
#include "node.h"

SubPNode::SubPNode() {
}

SubPNode::~SubPNode() {
}

void SubPNode::initialize() {

}

void SubPNode::handleMessage(cMessage *msg) {
    processMessage(msg);
}

void SubPNode::processMessage(cMessage *msg) {
}


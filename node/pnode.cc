#define FSM_DEBUG
#include "node.h"

PNode::PNode() {
}

PNode::~PNode() {
}

void PNode::initialize() {

}

void PNode::handleMessage(cMessage *msg) {
    processMessage(msg);
}

void PNode::processMessage(cMessage *msg) {
}


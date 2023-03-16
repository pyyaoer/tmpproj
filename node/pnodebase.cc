#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::sync(SyncMessage *smsg) {
    int gate_id = smsg->getGate_id();
    InfoMessage * msg = new InfoMessage();
    msg->setType(INFO_MESSAGE);
    send(msg, "edge_port$o", gate_id);
}

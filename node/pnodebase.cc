#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    id = par("id").intValue();
}

void PNodeBase::sync_edge(SyncMessage *smsg) {
    int edge_id = smsg->getEdge_id();
    InfoMessage * msg = new InfoMessage();
    msg->setType(INFO_MESSAGE);
    send(msg, "edge_port$o", edge_id);
}

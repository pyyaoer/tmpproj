#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    Node::initialize();
    tenant_n = par("tenant_n").intValue();
    local_edge_n = par("local_edge_n").intValue();
    bucket_size = par("bucket_size").intValue();
    local_leak_rate = par("local_leak_rate").doubleValue();
}

void PNodeBase::sync_edge(SyncMessage *smsg) {
    int edge_id = smsg->getEdge_id();
    InfoMessage * msg = new InfoMessage();
    for (int i = 0; i < local_edge_n; ++i) {
        msg->setBucket_size(i, bucket_size);
        msg->setLeak_rate(i, local_leak_rate / local_edge_n);
    }
    msg->setType(INFO_MESSAGE);
    send(msg, "edge_port$o", edge_id);
}

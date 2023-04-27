#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    Node::initialize();
    tenant_n = par("tenant_n").intValue();
    edge_n = par("edge_n").intValue();
    bucket_size = par("bucket_size").intValue();
    leak_rate = par("leak_rate").doubleValue();
}

void PNodeBase::sync_edge(SyncMessage *smsg) {
    int edge_id = smsg->getEdge_id();
    InfoMessage * msg = new InfoMessage();
    for (int i = 0; i < edge_n; ++i) {
        msg->setBucket_size(i, bucket_size);
        msg->setLeak_rate(i, leak_rate / edge_n);
    }
    msg->setType(INFO_MESSAGE);
    send(msg, "edge_port$o", edge_id);
}

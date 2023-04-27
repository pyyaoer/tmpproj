#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() : ratio_(TENANT_NUM) {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    Node::initialize();
    tenant_n = par("tenant_n").intValue();
    local_edge_n = par("local_edge_n").intValue();
    bucket_size = par("bucket_size").intValue();
    local_leak_rate = par("local_leak_rate").doubleValue();
    tenant_n = par("tenant_n").intValue();
}

void PNodeBase::sync_edge(SyncMessage *smsg) {
    int edge_id = smsg->getEdge_id();
    InfoMessage * msg = new InfoMessage();
    for (int i = 0; i < tenant_n; ++i) {
        double ratio = ratio_[i].UpdateAndGetRatio(edge_id, smsg->getCounter(i));
        msg->setBucket_size(i, bucket_size);
        msg->setLeak_rate(i, local_leak_rate * ratio);
    }
    msg->setType(INFO_MESSAGE);
    send(msg, "edge_port$o", edge_id);
}

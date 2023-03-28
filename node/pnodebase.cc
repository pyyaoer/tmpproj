#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() : record_r_(TENANT_NUM), record_l_(TENANT_NUM) {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    Node::initialize();
    tenant_n = par("tenant_n").intValue();
}

void PNodeBase::sync_edge(SyncMessage *smsg) {
    int edge_id = smsg->getEdge_id();
    InfoMessage * msg = new InfoMessage();
    msg->setType(INFO_MESSAGE);
    double period = smsg->getPeriod();
    for (int i = 0; i < TENANT_NUM; ++i) {
        int ri = smsg->getR(i);
        int li = smsg->getL(i);
        if (ri >= 0) {
            double n = record_r_[i].UpdateAndCount(edge_id, ri, period);
            msg->setRho(i, (ri == 0) ? 0 : n / ri);
        }
        if (li >= 0) {
            double n = record_l_[i].UpdateAndCount(edge_id, li, period);
            msg->setDelta(i, (li == 0) ? 0 : n / li);
        }
    }
    send(msg, "edge_port$o", edge_id);
}

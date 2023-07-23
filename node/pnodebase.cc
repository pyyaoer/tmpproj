#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() : record_r_(TENANT_NUM), record_l_(TENANT_NUM),
                        record_sub_l_(SUBP_NUM), record_sub_r_(SUBP_NUM) {
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::initialize() {
    Node::initialize();
    tenant_n = par("tenant_n").intValue();
    for (int i = 0; i < TENANT_NUM; ++i) {
        scaling_r[i] = 1;
        scaling_l[i] = 1;
    }
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
            double n = record_r_[i].update_count(edge_id, ri, period);
            msg->setRho(i, (ri == 0) ? 0 : n / ri * scaling_r[i]);
        }
        if (li >= 0) {
            double n = record_l_[i].update_count(edge_id, li, period);
            msg->setDelta(i, (li == 0) ? 0 : n / li * scaling_l[i]);
        }
    }
    send(msg, "edge_port$o", edge_id);
}

#define FSM_DEBUG
#include "node.h"

PNodeBase::PNodeBase() : record_r_(TENANT_NUM), record_l_(TENANT_NUM){
}

PNodeBase::~PNodeBase() {
}

void PNodeBase::sync(SyncMessage *smsg) {
    int gate_id = smsg->getGate_id();
    double period = smsg->getPeriod();
    InfoMessage * msg = new InfoMessage();
    msg->setType(INFO_MESSAGE);
    for (int i = 0; i < TENANT_NUM; ++i) {
        int ri = smsg->getR(i);
        int li = smsg->getL(i);
        if (ri >= 0) {
            double n = record_r_[i].UpdateAndCount(gate_id, ri, period);
            msg->setRho(i, (ri == 0) ? 0 : n / ri);
        }
        if (li >= 0) {
            double n = record_l_[i].UpdateAndCount(gate_id, li, period);
            msg->setDelta(i, (li == 0) ? 0 : n / li);
        }
    }
    send(msg, "edge_port$o", gate_id);
}

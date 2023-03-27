#include "node.h"

void m2::initialize() {
    window_size = par("window_size").doubleValue();
    for (int i = 0; i < TENANT_NUM; ++i) {
        lat_hist[i] = new cHistogram();
    }
}

void m2::update_latency(int tenant_id, double latency_data) {
    latency[tenant_id].push(latency_data);
    lat_vec[tenant_id].record(latency[tenant_id].get());
    lat_hist[tenant_id]->collect(latency[tenant_id].get());
    simtime_t now = simTime();
    iops_window[tenant_id].push(now);
    while (!iops_window[tenant_id].empty()) {
        if (iops_window[tenant_id].front() >= now - window_size) break;
        iops_window[tenant_id].pop();
    }
    iops_vec[tenant_id].record(iops_window[tenant_id].size() / window_size);
}

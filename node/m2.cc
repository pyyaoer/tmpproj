#include "node.h"

void m2::initialize() {
    for (int i = 0; i < TENANT_NUM; ++i)
        lat_hist[i] = new cHistogram();
}

void m2::update_latency(int tenant_id, double latency_data) {
    latency[tenant_id].push(latency_data);
    lat_vec[tenant_id].record(latency[tenant_id].get());
    lat_hist[tenant_id]->collect(latency[tenant_id].get());
}

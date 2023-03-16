#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

#define TENANT_NUM 8
#define GATE_NUM 64
#define EXECUTOR_NUM 16

typedef class Latency_t {
    std::queue<double> lat;
    double sum;
public:
    Latency_t(): sum(0) {}
    void push(double data) {
        sum += data;
        lat.push(data);
        if (lat.size() > 5) {
            sum -= lat.front();
            lat.pop();
        }
    }
    double get() {
        return sum / lat.size();
    }
} Latency;

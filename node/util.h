#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

#define TENANT_NUM 8
#define GATE_NUM 64
#define EXECUTOR_NUM 16

class Records {
    class Record {
    public:
        simtime_t period;
        double cnt;
        Record() : Record(0, -1) {}
        explicit Record(simtime_t p, int c)
        : period(p), cnt(c) {}
    };
public:
    std::vector<Record> records;
    explicit Records() : records(GATE_NUM) {}
    int UpdateAndCount(int gate, int cnt, double period) {
        records[gate] = Record(period, cnt);
        double s = 0;
        for (auto r : records) {
            s += r.cnt / r.period;
        }
        return s * period;
    }
};

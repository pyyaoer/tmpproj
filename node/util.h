#include <vector>
#include <omnetpp.h>

using namespace omnetpp;

#define TENANT_NUM 8
#define GATE_NUM 64

class Records {
    class Record {
    public:
        simtime_t start;
        simtime_t end;
        int cnt;
        Record() : Record(0, 0, -1) {}
        explicit Record(simtime_t s, simtime_t e, int c)
        : start(s), end(e), cnt(c) {}
    };
public:
    std::vector<Record> records;
    explicit Records() : records(GATE_NUM) {}
    int UpdateAndCount(int gate, int cnt, simtime_t start, simtime_t end) {
        records[gate] = Record(start, end, cnt);
        int s = 0;
        for (auto r : records) {
            if (r.cnt < 0) continue;
            if (start <= r.start and r.end <= end) {
                s += r.cnt;
            }
            else if (start <= r.end and r.end <= end) {
                // Assume that the requests follow a uniform distribution
                s += r.cnt * (r.end-start) / (r.end-r.start);
            }
        }
        return s;
    }
};

#include <vector>
#include <algorithm>
#include <omnetpp.h>

using namespace omnetpp;

static cValue repeat(cComponent *context, cValue argv[], int argc) {
    std::string str = argv[0].stdstringValue() + " ";
    int n = (int)argv[1];
    std::string ret;
    ret.reserve(str.size() * n);
    while (n--)
        ret += str;
    return ret;
}

Define_NED_Function(repeat, "string repeat(string str, int n)");

#define TENANT_NUM 8
#define EDGE_NUM 64
#define EXECUTOR_NUM 16

class Records {
    class Record {
    public:
        simtime_t period;
        double cnt;
        Record() : Record(1, 0) {}
        explicit Record(simtime_t p, int c)
        : period(p), cnt(c) {}
    };
public:
    std::vector<Record> records;
    explicit Records() : records(EDGE_NUM) {}
    double UpdateAndCount(int edge_id, int cnt, double period) {
        records[edge_id] = Record(period, cnt);
        double s = 0;
        for (auto r : records) {
            s += r.cnt / r.period;
        }
        return s * period;
    }
};

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

typedef struct {
    int task_id;
    int tenant_id;
    double duration;
    simtime_t creation;
    int iot;
} Task;

typedef struct TaskTime_t {
    int task_id;
    int tenant_id;
    simtime_t time;
    bool operator<(const TaskTime_t &y) const { return time > y.time; }
} TaskTime;

typedef struct VirtualTime {
    std::vector<TaskTime> timeline;
    void remove_item(int task_id) {
        if (task_id < 0) return;
        for (auto iter = timeline.begin(); iter != timeline.end(); iter++) {
            if (iter->task_id == task_id) {
                timeline.erase(iter);
                break;
            }
        }
        std::make_heap(timeline.begin(), timeline.end());
    }
    void insert_item(TaskTime t) {
        timeline.push_back(t);
        std::push_heap(timeline.begin(), timeline.end());
    }
    TaskTime pop_item() {
        if (timeline.empty()) return TaskTime({-1, -1, 0});
        std::pop_heap(timeline.begin(), timeline.end());
        TaskTime t = timeline.back();
        timeline.pop_back();
        return t;
    }
    bool has_ready() {
        if (timeline.empty()) return false;
        return timeline[0].time < simTime();
    }
} VirtualTime;

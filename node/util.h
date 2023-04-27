#include <vector>
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

typedef class Ratio_t {
    std::vector<int> past_;
    double sum;
public:
    Ratio_t(): sum(0), past_(EDGE_NUM) {
        for (auto &i : past_) {
            i = 0;
        }
    }
    double UpdateAndGetRatio(int index, int counter) {
        sum += counter - past_[index];
        past_[index] = counter;
        return sum == 0 ? 0 : counter / sum;
    }
} Ratio;

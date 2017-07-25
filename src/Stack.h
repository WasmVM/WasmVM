#include <Value.h>

class Label {
    uint64_t instrAddr;
};

class Frame {
    std::vector<Value> locals;

};
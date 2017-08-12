#ifndef STACK_DEF
#define STACK_DEF

#include <list>
#include <vector>
#include <Value.h>
#include <ModuleInst.h>

class Label {
public:
    uint32_t funcIdx;
    uint64_t instrOffset;
    bool loop;
    uint64_t loopOffset;
    std::vector<ValType> resultTypes;
    Label();
    Label(const Label&);
    Label &operator=(Label);
};

class Frame {
public:
    std::vector<Value> locals;
    ModuleInst *moduleInst;
    Frame();
    Frame(const Frame&);
    Frame &operator=(Frame);
};

enum StackElemType { value, label, frame };

class StackElem {
public:
    StackElemType type;
    void *data;
};

class Stack {
public:

    ~Stack();

    Label *curLabel = nullptr;
    
    void push(Value val);
    void push(Label lab);
    void push(Frame frm);
    StackElem &top();
    StackElem pop();
    size_t size();
    std::list<StackElem>::iterator end();
    std::list<StackElem>::iterator begin();
    size_t valueCount();
private:
    std::list<StackElem> _stack;
    inline void refreshLabel();
};

#endif
#ifndef STACK_DEF
#define STACK_DEF

#include <list>
#include <vector>
#include <cstdint>
#include <Value.h>
#include <ModuleInst.h>

class Label {
public:
    std::uint32_t funcIdx;
    std::uint64_t instrOffset;
    std::uint64_t contOffset;
    std::vector<ValType> resultTypes;
    Label(bool cont = true);
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
    void *data = nullptr;
};

class Stack {
public:

    ~Stack();

    Label *curLabel = nullptr;
    Frame *curFrame = nullptr;
    
    void push(Value val);
    void push(Label lab);
    void push(Frame frm);
    StackElem &top();
    StackElem pop();
    std::size_t size();
    std::list<StackElem>::iterator end();
    std::list<StackElem>::iterator begin();
    std::size_t valueCount();
private:
    std::list<StackElem> _stack;
};

#endif
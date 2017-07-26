#ifndef STACK_DEF
#define STACK_DEF

#include <stack>
#include <Value.h>
#include <ModuleInst.h>

class Label {
public:
    uint64_t instrAddr;
    Label();
    Label(const Label&);
    Label &operator=(Label);
};

class Frame {
public:
    std::vector<Value> locals;
    ModuleInst *moduleinst;
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
    void push(Value val);
    void push(Label lab);
    void push(Frame frm);
    StackElem &top();
    StackElem &pop();
    size_t size();

private:
    std::stack<StackElem> _stack;
};

#endif
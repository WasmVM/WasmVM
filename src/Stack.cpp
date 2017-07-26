#include <Stack.h>

Label::Label():instrAddr(0){
}
Label::Label(const Label& lab){
    instrAddr = lab.instrAddr;
}
Label& Label::operator=(Label lab){
    instrAddr = lab.instrAddr;
    return *this;
}

Frame::Frame():moduleinst(nullptr){
}
Frame::Frame(const Frame& frm){
    locals = frm.locals;
    moduleinst = frm.moduleinst;
}
Frame& Frame::operator=(Frame frm){
    locals = frm.locals;
    moduleinst = frm.moduleinst;
    return *this;
}

void Stack::push(Value val){
    StackElem elem;
    Value *inst = new Value(val);
    elem.type = value;
    elem.data = inst;
    _stack.push(elem);
}
void Stack::push(Label lab){
    StackElem elem;
    Label *inst = new Label(lab);
    elem.type = label;
    elem.data = inst;
    _stack.push(elem);
}
void Stack::push(Frame frm){
    StackElem elem;
    Frame *inst = new Frame(frm);
    elem.type = frame;
    elem.data = inst;
    _stack.push(elem);
}
StackElem &Stack::top(){
    return _stack.top();
}
StackElem &Stack::pop(){
    StackElem &elem = _stack.top();
    _stack.pop();
    return _stack.top();
}
size_t Stack::size(){
    return _stack.size();
}
#include <Stack.h>

/*
Copyright Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

Label::Label(): funcIdx(0), instrOffset(0), contOffset(0){
}
Label::Label(const Label& lab){
    funcIdx = lab.funcIdx;
    instrOffset = lab.instrOffset;
    resultTypes = lab.resultTypes;
    contOffset = lab.contOffset;
}
Label& Label::operator=(Label lab){
    funcIdx = lab.funcIdx;
    instrOffset = lab.instrOffset;
    resultTypes = lab.resultTypes;
    contOffset = lab.contOffset;
    return *this;
}

Frame::Frame():moduleInst(nullptr){
}
Frame::Frame(const Frame& frm){
    locals = frm.locals;
    moduleInst = frm.moduleInst;
}
Frame& Frame::operator=(Frame frm){
    locals = frm.locals;
    moduleInst = frm.moduleInst;
    return *this;
}
Stack::~Stack(){
    for(std::list<StackElem>::iterator stackIt = _stack.begin(); stackIt != _stack.end(); ++stackIt){
        switch(stackIt->type){
            case value:
                delete (Value *)stackIt->data;
            break;
            case label:
                delete (Label *)stackIt->data;
            break;
            case frame:
                delete (Frame *)stackIt->data;
            break;
        }
    }
}
void Stack::push(Value val){
    StackElem elem;
    Value *inst = new Value(val);
    elem.type = value;
    elem.data = inst;
    ++valueNum;
    _stack.push_front(elem);
}
void Stack::push(Label lab){
    StackElem elem;
    Label *inst = new Label(lab);
    elem.type = label;
    elem.data = inst;
    _stack.push_front(elem);
    valueNum = 0;
    curLabel = inst;
}
void Stack::push(Frame frm){
    StackElem elem;
    Frame *inst = new Frame(frm);
    elem.type = frame;
    elem.data = inst;
    _stack.push_front(elem);
    curFrame = inst;
}
StackElem &Stack::top(){
    return *(_stack.begin());
}
StackElem Stack::pop(){
    StackElem elem = *(_stack.begin());
    _stack.pop_front();
    if(elem.type == label){
        valueNum = 0;
        for(std::list<StackElem>::iterator stackIt = _stack.begin(); stackIt != _stack.end(); ++stackIt){
            if(stackIt->type == label){
                curLabel = (Label *)stackIt->data;
                return elem;
            }else if(stackIt->type == frame){
                curLabel = nullptr;
                return elem;
            }else{
                ++valueNum;
            }
        }
        curLabel = nullptr;
    }else if(elem.type == frame){
        for(std::list<StackElem>::iterator stackIt = _stack.begin(); stackIt != _stack.end(); ++stackIt){
            if(stackIt->type == label && curLabel == nullptr){
                curLabel = (Label *)stackIt->data;
            }else if(stackIt->type == frame){
                curFrame = (Frame *)stackIt->data;
                return elem;
            }
        }
        curFrame = nullptr;
    }else{
        --valueNum;
    }
    return elem;
}
size_t Stack::size(){
    return _stack.size();
}
std::list<StackElem>::iterator Stack::end(){
    return _stack.end();
}
std::list<StackElem>::iterator Stack::begin(){
    return _stack.begin();
}
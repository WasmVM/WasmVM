#ifndef WASMVM_CORE_STACK_DEF
#define WASMVM_CORE_STACK_DEF

#include <dataTypes/stack.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Value.h>

typedef struct {
    stack* entries; // Entry
    Label curLabel; // current Label
    Frame curFrame; // current Frame
} Stack;

Stack* new_Stack();
void free_Stack(Stack* thisStack);
void push_Label(Stack* thisStack, Label label);
void push_Frame(Stack* thisStack, Frame frame);
void push_Value(Stack* thisStack, Value* value);
int pop_Label(Stack* thisStack, Label* label);
int pop_Frame(Stack* thisStack, Frame* framePtr);
int pop_Value(Stack* thisStack, Value** value);

#endif

#ifndef WASMVM_CORE_STACK_DEF
#define WASMVM_CORE_STACK_DEF

#include <stdint.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Value.h>

typedef struct Stack_ * Stack;

Stack new_Stack();
void free_Stack(Stack stack);
void push_Label(Stack stack, Label label);
void push_Frame(Stack stack, Frame frame);
void push_Value(Stack stack, Value* value);
int pop_Label(Stack stack, Label* label, _Bool restoreValue);
int pop_Frame(Stack stack, Frame* framePtr, Label lastLabel);
int pop_Value(Stack stack, Value** value);

Label stack_cur_label(Stack stack);
Frame stack_cur_frame(Stack stack);
Entry* stack_top_entry(Stack stack);

#endif

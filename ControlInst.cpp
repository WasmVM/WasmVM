#include "ControlInst.h"

#include <cstdio>

void Instruction::ctrl_nop (){
}
void Instruction::ctrl_unreachable(){
}
void Instruction::ctrl_block(){
}
void Instruction::ctrl_loop(){
}
void Instruction::ctrl_if(){
}
void Instruction::ctrl_br(){
}
void Instruction::ctrl_br_if(){
}
void Instruction::ctrl_return(){
}
void Instruction::ctrl_call(uint32_t entry,
  OperandStack &stack,
  LocalStack &locals,
  Memory &memory,
  std::stack<uint64_t> &pcStack){
  // Set OperandStack
  stack.append_Stack();
  // Set LocalStack
  locals.append_Values();
  locals.append_Indices();
  // Set PC
  pcStack.push(memory.code_elements[entry].func_mem_loc);
  // Set Params
  // TODO: Set Params
  // Set Locals
  int localDeclCnt = memory.i32_load8_u(++pcStack.top());
  for(int i = 0; i < localDeclCnt; ++i){
    int localTypeCnt = memory.i32_load8_u(++pcStack.top());
    int localType = memory.i32_load8_u(++pcStack.top());
    Value newVal;
    newVal.data.i64 = 0;
    switch (localType){
    case 0x7f:
      newVal.type = i32;
      break;
    case 0x7e:
      newVal.type = i64;
      break;
    case 0x7d:
      newVal.type = f32;
      break;
    case 0x7c:
      newVal.type = f64;
      break;
    default:
      continue;
    }
    for(int j = 0; j < localTypeCnt; ++j){
      locals.push_local(newVal);
    }
  }
  ++pcStack.top();
}
void Instruction::ctrl_end(){
}
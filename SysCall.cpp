#include "SysCall.h"

void SystemCall::handle(OperandStack &stack, Memory &memory){
  Value code = stack.pop();
  switch (code.data.i32){
  case 0:
    printOperand(stack);
    break;
  default:
    printf("System Halted: Undefined system call (%d)\n", code.data.i32);
    exit(-1);
  }
}

void SystemCall::printOperand(OperandStack &stack){
  
}
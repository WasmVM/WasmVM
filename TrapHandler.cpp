#include "TrapHandler.h"

void TrapHandler::handle(OperandStack &stack, Memory &memory){
  Value code = stack.pop();
  switch (code.data.i32){
  case 0:
    printf(stack, memory);
    break;
  default:
    printf("System Halted: Undefined trap (%d)\n", code.data.i32);
    exit(-1);
  }
}

void TrapHandler::printf(OperandStack &stack, Memory &memory){
  
}
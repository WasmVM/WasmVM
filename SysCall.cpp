#include "SysCall.h"

SystemCall::SystemCall(OperandStack &opStack, LocalStack &locals, Memory &memory){
  Value code = opStack._stacks.top().top();
  switch (code.data.i32){
  case 0:
    printOperand(opStack);
    break;
  default:
    printf("System Halted: Undefined system call (%d)\n", code.data.i32);
    exit(-1);
  }
}

void SystemCall::printOperand(OperandStack &opStack){
  printf("blah\n");
}
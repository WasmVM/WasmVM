#include "ParaVarInst.h"

#include <cstdio>

void Instruction::para_drop(OperandStack &opStack){
  Value val = opStack.pop();
  ValueType type = val.type;
  switch (type){
    case i32:
      printf("Operand dropped: Type: i32, Value: %d\n", val.data.i32);
      break;
    case i64:
      printf("Operand dropped: Type: i64, Value: %ld\n", val.data.i64);
      break;
    default:
      break;
  }
}
void Instruction::para_select(OperandStack &opStack){
  Value val1, val2, val3;
  val3 = opStack.pop();
  val2 = opStack.pop();
  val1 = opStack.pop();
  if(val3.data.i32){
    opStack.push(val1);
  }else{
    opStack.push(val2);
  }
}
void Instruction::get_local(){
}
void Instruction::set_local(){
}
void Instruction::tee_local(){
}
void Instruction::get_global(){
}
void Instruction::set_global(){
}
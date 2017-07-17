#include "ParaVarInst.h"

#ifdef DEBUG
#include <cstdio>
#endif

void Instruction::para_drop(OperandStack &opStack){
  Value val = opStack.popVal();
  #ifdef DEBUG
  ValueType type = val.type;
  switch (type){
    case i32:
      printf("Operand dropped: Type: i32, Value: %d\n", val.data.i32);
      break;
    case i64:
      printf("Operand dropped: Type: i64, Value: %ld\n", val.data.i64);
      break;
    case f32:
      printf("Operand dropped: Type: f32, Value: %f\n", val.data.f32);
      break;
    case f64:
      printf("Operand dropped: Type: f64, Value: %lf\n", val.data.f64);
      break;
    default:
      break;
  }
  #endif
}
void Instruction::para_select(OperandStack &opStack){
  Value val1, val2, val3;
  val3 = opStack.popVal();
  val2 = opStack.popVal();
  val1 = opStack.popVal();
  if(val3.data.i32){
    opStack.pushVal(val1);
  }else{
    opStack.pushVal(val2);
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
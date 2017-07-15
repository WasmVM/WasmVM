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
void Instruction::get_local(OperandStack &opStack, LocalStack &locals, Memory memory){
  uint32_t index = Common::get_uleb128_32(memory, locals.get_PC());
  Value val = locals.get_local(index);
  opStack.pushVal(val);
}
void Instruction::set_local(OperandStack &opStack, LocalStack &locals, Memory memory){
  uint32_t index = Common::get_uleb128_32(memory, locals.get_PC());
  Value val = opStack.popVal();
  locals.set_local(index, val);
}
void Instruction::tee_local(OperandStack &opStack, LocalStack &locals, Memory memory){
  uint32_t index = Common::get_uleb128_32(memory, locals.get_PC());
  Value val = opStack.popVal();
  opStack.pushVal(locals.tee_local(index, val));
}
void Instruction::get_global(){
}
void Instruction::set_global(){
}
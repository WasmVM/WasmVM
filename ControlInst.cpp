#include "ControlInst.h"

void Instruction::ctrl_nop (){
}
void Instruction::ctrl_unreachable(OperandStack &opStack, LocalStack &locals, Memory &memory){
  SystemCall sysCall(opStack, locals, memory);
}
void Instruction::ctrl_block(LocalStack &locals, uint32_t retType){
  BlockExtra *extra = new BlockExtra;
  extra->retType = retType;
  locals.push_index(i_block, extra);
}
void Instruction::ctrl_loop(LocalStack &locals, uint32_t retType){
  LoopExtra *extra = new LoopExtra;
  extra->pc = locals.get_PC() - 2;
  extra->retType = retType;
  locals.push_index(i_loop, extra);
}
void Instruction::ctrl_if(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t retType){
  // Get PC
  uint64_t &pc = locals.get_PC();
  // Extra
  IfExtra *extra = new IfExtra;
  extra->endPos = pc;
  extra->elsePos = 0;
  extra->retType = retType;
  if(bypass(memory, extra->endPos) == 0x05){
    extra->elsePos = extra->endPos;
    bypass(memory, ++extra->endPos);
  }
  locals.push_index(i_if, extra);
  // Get value
  Value val = opStack.pop();
  if(!val.data.i32){
    pc = extra->elsePos + 1;
  }
}
void Instruction::ctrl_else(LocalStack &locals){
  LocalIndex index = locals.pop_index();
  IfExtra *extra = (IfExtra *)index.extra;
  locals.get_PC() = extra->endPos + 1;
  delete extra;
}
void Instruction::ctrl_br(LocalStack &locals, Memory &memory, uint32_t label){
  while(label--){
    LocalIndex index = locals.pop_index();
    if(index.type == i_function){
      //TODO: exception
      break;
    }
    if(index.type == i_block){
      BlockExtra *extra = (BlockExtra *)index.extra;
      delete extra;
      bypass(memory, locals.get_PC());
      break;
    }
    if(index.type == i_loop){
      LoopExtra *extra = (LoopExtra *)index.extra;
      delete extra;
      bypass(memory, locals.get_PC());
    }
    if(index.type == i_if){
      IfExtra *extra = (IfExtra *)index.extra;
      locals.get_PC() = extra->endPos;
      delete extra;
    }
  }
}
void Instruction::ctrl_br_if(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t label){
  if(opStack.pop().data.i32){
    ctrl_br(locals, memory, label);
  }
}
void Instruction::ctrl_return(){
}
void Instruction::ctrl_call(uint32_t entry,
  OperandStack &opStack,
  LocalStack &locals,
  Memory &memory){
  // Set OperandStack
  opStack.append();
  // Set LocalStack & PC
  locals.append(memory.code_elements[entry].func_mem_loc);
  // Get PC
  uint64_t &pc = locals.get_PC();
  // Set Params
  // TODO: Set Params
  // Set Locals
  int localDeclCnt = memory.i32_load8_u(++pc);
  for(int i = 0; i < localDeclCnt; ++i){
    int localTypeCnt = memory.i32_load8_u(++pc);
    int localType = memory.i32_load8_u(++pc);
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
  ++pc;
}
void Instruction::ctrl_end(OperandStack &opStack, LocalStack &locals, bool &halted){
  LocalIndex index = locals.pop_index();
  if(index.type == i_function){
    opStack.shrink();
    halted = locals.shrink();
  }
  if(index.type == i_block){
    BlockExtra *extra = (BlockExtra *)index.extra;
    delete extra;
  }
  if(index.type == i_loop){
    LoopExtra *extra = (LoopExtra *)index.extra;
    locals.get_PC() = extra->pc;
    delete extra;
  }
  if(index.type == i_if){
    IfExtra *extra = (IfExtra *)index.extra;
    delete extra;
  }
}

uint32_t Instruction::bypass(Memory &memory, uint64_t &pc){
  int endCount = 0;
  while(1){
    uint32_t opCode = memory.i32_load8_u(pc);
    if(endCount){
      if(opCode == 0x0B){ // end
        endCount--;
      }
    }else{
      if(opCode == 0x0B || opCode == 0x05){
        return opCode;
      }
    }
    switch (opCode){
    case 0x03: // loop
    case 0x04: // if
      endCount++;
      memory.i32_load8_u(++pc);
    break;
    case 0x41: // i32.const
      Common::get_leb128_32(memory, ++pc);
    break;
    case 0x42: // i64.const
      Common::get_leb128_64(memory, ++pc);
    break;
    default:
      break;
    }
    pc++;
  }
}
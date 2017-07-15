#include "ControlInst.h"

void Instruction::ctrl_nop (){
}
void Instruction::ctrl_unreachable(OperandStack &opStack, LocalStack &locals, Memory &memory){
  SystemCall sysCall(opStack, locals, memory);
}
void Instruction::ctrl_block(OperandStack &opStack, LocalStack &locals, uint32_t retType){
  opStack.append();
  BlockExtra *extra = new BlockExtra;
  extra->retType = retType;
  locals.push_index(i_block, extra);
}
void Instruction::ctrl_loop(OperandStack &opStack, LocalStack &locals, uint32_t retType){
  opStack.append();
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
  Value val = opStack.popVal();
  if(!val.data.i32){
    pc = extra->elsePos + 1;
  }
  // Append stack
  opStack.append();
}
void Instruction::ctrl_else(LocalStack &locals){
  LocalIndex index = locals.peek_index();
  IfExtra *extra = (IfExtra *)index.extra;
  locals.get_PC() = extra->endPos;
}
void Instruction::ctrl_br(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t label, bool &halted){
  while(label--){
    LocalIndex index = locals.peek_index();
    if(index.type == i_function){
      throw "Can't branch from function";
      break;
    }
    if(index.type == i_block){
      bypass(memory, locals.get_PC());
      locals.get_PC() += 1;
    }
    if(index.type == i_loop){
      bypass(memory, locals.get_PC());
      LoopExtra *extra = (LoopExtra *)index.extra;
      extra->pc = locals.get_PC() + 1;
    }
    if(index.type == i_if){
      IfExtra *extra = (IfExtra *)index.extra;
      locals.get_PC() = extra->endPos + 1;
    }
    ctrl_end(opStack, locals, halted);
  }
}
void Instruction::ctrl_br_if(OperandStack &opStack, LocalStack &locals, Memory &memory, uint32_t label, bool &halted){
  if(opStack.popVal().data.i32){
    ctrl_br(opStack, locals, memory, label, halted);
  }
}
void Instruction::ctrl_br_table(OperandStack &opStack, LocalStack &locals, Memory &memory, bool &halted){
  uint64_t &pc = locals.get_PC();
  // Get target count
  uint32_t targetCount = Common::get_uleb128_32(memory, pc);
  vector<uint32_t> targets;
  for(uint32_t i = 0; i < targetCount; ++i){
    targets.push_back(Common::get_uleb128_32(memory, pc));
  }
  // Default target
  targets.push_back(Common::get_uleb128_32(memory, pc));
  Value val = opStack.popVal();
  if(val.data.i32 < 0 || val.data.i32 >= targetCount){
    ctrl_br(opStack, locals, memory, targets.at(targets.size() - 1), halted);
  }else{
    ctrl_br(opStack, locals, memory, targets.at(val.data.i32), halted);
  }
}
void Instruction::ctrl_return(OperandStack &opStack, LocalStack &locals, bool &halted){
  ctrl_end(opStack, locals, halted);
}
void Instruction::ctrl_call(uint32_t entry,
  OperandStack &opStack,
  LocalStack &locals,
  Memory &memory){
  // Set LocalStack & PC
  locals.append(memory.code_elements[entry].func_mem_loc);
  // Get result info & Push LocalIndex
  FuncExtra *extra = new FuncExtra;
  int sigIndex = memory.funcs_elements[entry].func_signature_index;
  for(int i = 0; i < memory.type_elements[sigIndex].num_result; ++i){
    extra->retTypes.push_back(memory.type_elements[sigIndex].result_type.at(i));
  }
  locals.push_index(i_function, extra);
  // Get PC
  uint64_t &pc = locals.get_PC();
  // Get Params
  vector<Value> params;
  for(int i = 0; i < memory.type_elements[sigIndex].num_param; ++i){
    Value val = opStack.popVal();
    switch(memory.type_elements[sigIndex].param_type.at(i)){
      case 0x7f:
        if(val.type != i32){throw "Param type not match i32";}
        break;
      case 0x7e:
        if(val.type != i64){throw "Param type not match i64";}
        break;
      case 0x7d:
        if(val.type != f32){throw "Param type not match f32";}
        break;
      case 0x7c:
        if(val.type != f64){throw "Param type not match f64";}
        break;
      default:
        throw "No such param type";
    }
    params.push_back(val);
  }
  // Set OperandStack
  opStack.append();
  // Set Params
  for(int i = params.size() - 1; i >= 0; --i){
    locals.push_local(params.at(i));
  }
  // Set Locals
  int localDeclCnt = memory.i32_load8_u(++pc);
  for(int i = 0; i < localDeclCnt; ++i){
    uint32_t localTypeCnt = memory.i32_load8_u(++pc);
    uint32_t localType = memory.i32_load8_u(++pc);
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
    for(uint32_t j = 0; j < localTypeCnt; ++j){
      locals.push_local(newVal);
    }
  }
  ++pc;
}
void Instruction::ctrl_call_indirect(uint32_t typeIndex, OperandStack &opStack, LocalStack &locals, Memory &memory){
  locals.get_PC() += 1; // call_indirect reserved
  // Get expect type
  types expectType = memory.type_elements[typeIndex];
  // Get element index
  Value elemVal = opStack.popVal();
  if(elemVal.type != i32){
    throw "Wrong element type";
  }
  uint32_t elemIndex = elemVal.data.i32;
  // Get function entry
  uint32_t entry = -1;
  for(int i = 0; i < memory.element_elements.size(); i++)
  {
    if(elemIndex < memory.element_elements[i].element_indices.size()){
      entry = memory.element_elements[i].element_indices.at(elemIndex);
    }else{
      elemIndex -= memory.element_elements[i].element_indices.size();
      continue;
    }
  }
  if(entry == -1){
    throw "Function index out of range";
  }
  // Set LocalStack & PC
  locals.append(memory.code_elements[entry].func_mem_loc);
  // Get result info & Push LocalIndex
  FuncExtra *extra = new FuncExtra;
  int sigIndex = memory.funcs_elements[entry].func_signature_index;
  for(int i = 0; i < memory.type_elements[sigIndex].num_result; ++i){
    extra->retTypes.push_back(memory.type_elements[sigIndex].result_type.at(i));
  }
  locals.push_index(i_function, extra);
  // Get PC
  uint64_t &pc = locals.get_PC();
  // Get Params
  vector<Value> params;
  for(int i = 0; i < memory.type_elements[sigIndex].num_param; ++i){
    Value val = opStack.popVal();
    switch(memory.type_elements[sigIndex].param_type.at(i)){
      case 0x7f:
        if(val.type != i32){throw "Param type not match i32";}
        break;
      case 0x7e:
        if(val.type != i64){throw "Param type not match i64";}
        break;
      case 0x7d:
        if(val.type != f32){throw "Param type not match f32";}
        break;
      case 0x7c:
        if(val.type != f64){throw "Param type not match f64";}
        break;
      default:
        throw "No such param type";
    }
    params.push_back(val);
  }
  // Set OperandStack
  opStack.append();
  // Set Params
  for(int i = params.size() - 1; i >= 0; --i){
    locals.push_local(params.at(i));
  }
  // Set Locals
  int localDeclCnt = memory.i32_load8_u(++pc);
  for(int i = 0; i < localDeclCnt; ++i){
    uint32_t localTypeCnt = memory.i32_load8_u(++pc);
    uint32_t localType = memory.i32_load8_u(++pc);
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
    for(uint32_t j = 0; j < localTypeCnt; ++j){
      locals.push_local(newVal);
    }
  }
  ++pc;
}
void Instruction::ctrl_end(OperandStack &opStack, LocalStack &locals, bool &halted){
  LocalIndex index = locals.pop_index();
  if(index.type == i_function){
    vector<Value> retVals;
    FuncExtra *extra = (FuncExtra *)index.extra;
    for(uint32_t i = 0; i < extra->retTypes.size(); ++i){
      Value val = opStack.popVal();
      switch (extra->retTypes.at(i)){
      case 0x7f:
        if(val.type == i32){
          retVals.push_back(val);
        }else{
          throw "Return type not match i32";
        }
        break;
      case 0x7e:
        if(val.type == i64){
          retVals.push_back(val);
        }else{
          throw "Return type not match i64";
        }
        break;
      case 0x7d:
        if(val.type == f32){
          retVals.push_back(val);
        }else{
          throw "Return type not match f32";
        }
        break;
      case 0x7c:
        if(val.type == f64){
          retVals.push_back(val);
        }else{
          throw "Return type not match f64";
        }
        break;
      default:
        throw "No such types";
      }
    }
    delete extra;
    opStack.shrink();
    for(uint32_t i = 0; i < retVals.size(); ++i){
      opStack.pushVal(retVals.at(i));
    }
    halted = locals.shrink();
  }
  if(index.type == i_block){
    BlockExtra *extra = (BlockExtra *)index.extra;
    if(extra->retType == 0x40){
      opStack.shrink();
    }else{
      Value retVal = opStack.popVal();
      switch (extra->retType){
      case 0x7f:
        if(retVal.type != i32){
          throw "Return type not match i32";
        }
        break;
      case 0x7e:
        if(retVal.type != i64){
          throw "Return type not match i64";
        }
        break;
      case 0x7d:
        if(retVal.type != f32){
          throw "Return type not match f32";
        }
        break;
      case 0x7c:
        if(retVal.type != f64){
          throw "Return type not match f64";
        }
        break;
      default:
        throw "No such types";
      }
      opStack.shrink();
      opStack.pushVal(retVal);
    }
    delete extra;
  }
  if(index.type == i_loop){
    LoopExtra *extra = (LoopExtra *)index.extra;
    if(extra->retType == 0x40){
      opStack.shrink();
    }else{
      Value retVal = opStack.popVal();
      switch (extra->retType){
      case 0x7f:
        if(retVal.type != i32){
          throw "Return type not match i32";
        }
        break;
      case 0x7e:
        if(retVal.type != i64){
          throw "Return type not match i64";
        }
        break;
      case 0x7d:
        if(retVal.type != f32){
          throw "Return type not match f32";
        }
        break;
      case 0x7c:
        if(retVal.type != f64){
          throw "Return type not match f64";
        }
        break;
      default:
        throw "No such types";
      }
      opStack.shrink();
      opStack.pushVal(retVal);
    }
    locals.get_PC() = extra->pc;
    delete extra;
  }
  if(index.type == i_if){
    IfExtra *extra = (IfExtra *)index.extra;
    if(extra->retType == 0x40){
      opStack.shrink();
    }else{
      Value retVal = opStack.popVal();
      switch (extra->retType){
      case 0x7f:
        if(retVal.type != i32){
          throw "Return type not match i32";
        }
        break;
      case 0x7e:
        if(retVal.type != i64){
          throw "Return type not match i64";
        }
        break;
      case 0x7d:
        if(retVal.type != f32){
          throw "Return type not match f32";
        }
        break;
      case 0x7c:
        if(retVal.type != f64){
          throw "Return type not match f64";
        }
        break;
      default:
        throw "No such types";
      }
      opStack.shrink();
      opStack.pushVal(retVal);
    }
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
    case 0x02: // block
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
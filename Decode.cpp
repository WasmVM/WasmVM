#include "Decode.h"

#include <cstdio>

void Decode::decode(Memory &memory, OperandStack &opStack, LocalStack &locals, bool &halted){
  // Get opcode
  uint64_t &pc = locals.get_PC();
  uint32_t opCode = memory.i32_load8_u(pc++);
  switch (opCode){
  case 0x1A:
    Instruction::para_drop(opStack);
    break;
  case 0x1B:
    Instruction::para_select(opStack);
    printf("%d\n", opStack.pop().data.i32);
    halted = true;
    break;
  case 0x41:
    Instruction::i32_const(opStack, get_leb128_32(memory, pc));
    break;
  case 0x42:
    Instruction::i64_const(opStack, get_leb128_64(memory, pc));
    break;
  default:
    throw "Not implement yet";
    break;
  }
}

uint32_t Decode::get_uleb128_32(Memory &memory, uint64_t &pc){
  uint32_t ret = 0;
  for(int i = 0; i < 5; ++i){
    int byte = memory.i32_load8_u(pc++);
    ret |= (byte & 0x7F) << (7 * i);
    // Check range
    if(i == 4){
      if(byte & 0x80){
        throw "Too much bytes while decode uleb128_32.";
      }
      if(byte & 0xF0){
        throw "Overflow occured while decode uleb128_32.";
      }
    }
    // Finished
    if((byte & 0x80) == 0){
      break;
    }
  }
  return ret;
}

int32_t Decode::get_leb128_32(Memory &memory, uint64_t &pc){
  int32_t ret = 0;
  for(int i = 0; i < 5; ++i){
    int byte = memory.i32_load8_u(pc++);
    ret |= (byte & 0x7F) << (7 * i);
    // Check range
    if(i == 4){
      if(byte & 0x80){
        throw "Too much bytes while decode leb128_32.";
      }
      if(byte & 0xF0){
        throw "Overflow occured while decode leb128_32.";
      }
    }
    // Finished
    if((byte & 0x80) == 0){
      if(byte & 0x40){
        switch (i){
          case 0:
            ret |= 0xFFFFFF80;
            break;
          case 1:
            ret |= 0xFFFFC000;
            break;
          case 2:
            ret |= 0xFFE00000;
            break;
          case 3:
            ret |= 0xF0000000;
            break;
          default:
            break;
        }
      }
      break;
    }
  }
  return ret;
}

int64_t Decode::get_leb128_64(Memory &memory, uint64_t &pc){
  uint64_t ret = 0;
  for(int i = 0; i < 10; ++i){
    int64_t byte = memory.i64_load8_u(pc++);
    ret |= (byte & 0x7F) << (7 * i);
    // Check range
    if(i == 9){
      if(byte & 0x80){
        throw "Too much bytes while decode uleb128_64.";
      }
      if(byte & 0xFE){
        throw "Overflow occured while decode uleb128_64.";
      }
    }
    // Finished
    if((byte & 0x80) == 0){
      if(byte & 0x40){
        switch (i){
          case 0:
            ret |= 0xFFFFFFFFFFFFFF80;
            break;
          case 1:
            ret |= 0xFFFFFFFFFFFFC000;
            break;
          case 2:
            ret |= 0xFFFFFFFFFFE00000;
            break;
          case 4:
            ret |= 0xFFFFFFFFF0000000;
            break;
          case 5:
            ret |= 0xFFFFFFF800000000;
            break;
          case 6:
            ret |= 0xFFFFFC0000000000;
            break;
          case 7:
            ret |= 0xFFFE000000000000;
            break;
          case 8:
            ret |= 0xFF00000000000000;
            break;
          default:
            break;
        }
      }
      break;
    }
  }
  return ret;
}
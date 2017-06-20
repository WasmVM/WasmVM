#include "Decode.h"

#include <cstdio>

void Decode::decode(Memory &memory, stack<uint64_t> pcStack){
  // Get opcode
  uint32_t opCode = memory.i32_load8_u(pcStack.top()++);
  printf("op: %X, pc: %x\n", opCode, pcStack.top());
  switch (opCode){
  case 0x41:
    printf("LEB: %u\n", get_uleb128_32(memory, pcStack.top()));
    break;
  default:

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

}
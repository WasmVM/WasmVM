#ifndef MEMORY_INST
#define MEMORY_INST

#include "commonTypes.h"
#include "OperandStack.h"
#include "LocalStack.h"
#include "Memory.h"

namespace Instruction{
  // integer 
  void i32_load8_u (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i32_load16_u (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i32_load (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i64_load8_u (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i64_load16_u (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i64_load32_u (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i64_load (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i32_store (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void i64_store (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  // float
  void f32_load (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void f64_load (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void f32_store (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
  void f64_store (OperandStack &opStack, Memory &memory, uint32_t offset, uint32_t align);
}

#endif
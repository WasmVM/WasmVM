#ifndef MEMORYINSTR_DEF
#define MEMORYINSTR_DEF

#include <cstdint>
#include <new>
#include <Stack.h>
#include <Store.h>
#include <Instance.h>
#include <Exception.h>
#include <Util.h>

namespace Instruction {
void i32_load(std::uint32_t align,
              std::uint32_t offset,
              Store& store,
              Stack& coreStack);
void i64_load(std::uint32_t align,
              std::uint32_t offset,
              Store& store,
              Stack& coreStack);
void f32_load(std::uint32_t align,
              std::uint32_t offset,
              Store& store,
              Stack& coreStack);
void f64_load(std::uint32_t align,
              std::uint32_t offset,
              Store& store,
              Stack& coreStack);
void i32_load8_s(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i32_load8_u(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i32_load16_s(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);
void i32_load16_u(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);
void i64_load8_s(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i64_load8_u(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i64_load16_s(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);
void i64_load16_u(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);
void i64_load32_s(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);
void i64_load32_u(std::uint32_t align,
                  std::uint32_t offset,
                  Store& store,
                  Stack& coreStack);

void i32_store(std::uint32_t align,
               std::uint32_t offset,
               Store& store,
               Stack& coreStack);
void i64_store(std::uint32_t align,
               std::uint32_t offset,
               Store& store,
               Stack& coreStack);
void f32_store(std::uint32_t align,
               std::uint32_t offset,
               Store& store,
               Stack& coreStack);
void f64_store(std::uint32_t align,
               std::uint32_t offset,
               Store& store,
               Stack& coreStack);
void i32_store8(std::uint32_t align,
                std::uint32_t offset,
                Store& store,
                Stack& coreStack);
void i32_store16(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i64_store8(std::uint32_t align,
                std::uint32_t offset,
                Store& store,
                Stack& coreStack);
void i64_store16(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void i64_store32(std::uint32_t align,
                 std::uint32_t offset,
                 Store& store,
                 Stack& coreStack);
void current_memory(Store& store, Stack& coreStack);
void grow_memory(Store& store, Stack& coreStack);
}

#endif
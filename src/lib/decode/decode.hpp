#ifndef WASMVM_PP_DECODE_DEF
#define WASMVM_PP_DECODE_DEF

#include <WasmVM.hpp>
#include <istream>

namespace WasmVM {
namespace Decode {

constexpr u32_t magic = 0x6d736100;
constexpr u32_t version = 0x01;

} // namespace Decode
} // namespace WasmVM

#endif
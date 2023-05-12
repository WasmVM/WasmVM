#ifndef WASMVM_PP_PARSE_NODES_INSTR_NUMERIC_DEF
#define WASMVM_PP_PARSE_NODES_INSTR_NUMERIC_DEF

#include "../../nodes.hpp"

namespace WasmVM {
namespace Parse {

struct Numeric_i32 {
    Numeric_i32(WasmVM::Parse::Instr::NumericInstr numeric) : numeric(numeric){}
    static std::optional<Numeric_i32> get(TokenIter& begin, TokenHolder& holder);
    WasmVM::Parse::Instr::NumericInstr numeric;
};

struct Numeric_i64 {
    Numeric_i64(WasmVM::Parse::Instr::NumericInstr numeric) : numeric(numeric){}
    static std::optional<Numeric_i64> get(TokenIter& begin, TokenHolder& holder);
    WasmVM::Parse::Instr::NumericInstr numeric;
};

struct Numeric_f32 {
    Numeric_f32(WasmVM::Parse::Instr::NumericInstr numeric) : numeric(numeric){}
    static std::optional<Numeric_f32> get(TokenIter& begin, TokenHolder& holder);
    WasmVM::Parse::Instr::NumericInstr numeric;
};

struct Numeric_f64 {
    Numeric_f64(WasmVM::Parse::Instr::NumericInstr numeric) : numeric(numeric){}
    static std::optional<Numeric_f64> get(TokenIter& begin, TokenHolder& holder);
    WasmVM::Parse::Instr::NumericInstr numeric;
};

}
}

#endif
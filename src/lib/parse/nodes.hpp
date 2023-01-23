#ifndef WASMVM_PP_PARSE_NODES_DEF
#define WASMVM_PP_PARSE_NODES_DEF

#include <Types.hpp>
#include "parse.hpp"
#include <structures/WasmImport.hpp>

namespace WasmVM {
namespace Parse {

struct FuncType : public WasmVM::FuncType {
    static std::optional<FuncType> get(TokenIter& begin, const TokenIter& end);
};

struct ValueType {
    ValueType(WasmVM::ValueType type) : type(type){}
    static std::optional<ValueType> get(TokenIter& begin, const TokenIter& end);
    WasmVM::ValueType type;
};

struct Import {
    Import(WasmVM::WasmImport import) : import(import){}
    static std::optional<Import> get(TokenIter& begin, const TokenIter& end);
    WasmVM::WasmImport import;
};

}
}
#endif
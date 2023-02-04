#ifndef WASMVM_PP_PARSE_NODES_DEF
#define WASMVM_PP_PARSE_NODES_DEF

#include <Types.hpp>
#include "parse.hpp"
#include <structures/WasmImport.hpp>

#include <map>

namespace WasmVM {
namespace Parse {

struct Index : public OneOf<Token::Number, Token::Id> {
    template <typename T>
    T& retrieve(std::vector<T>& container);
};

struct FuncType : public WasmVM::FuncType {
    static std::optional<FuncType> get(TokenIter& begin, const TokenIter& end);
    std::map<std::string, index_t> id_map;
};

struct Type {
    static std::optional<Type> get(TokenIter& begin, const TokenIter& end);
    std::variant<std::string, index_t> id;
    FuncType func;
    Token::Location location;
};

struct ValueType {
    ValueType(WasmVM::ValueType type) : type(type){}
    static std::optional<ValueType> get(TokenIter& begin, const TokenIter& end);
    operator WasmVM::ValueType();
private:
    WasmVM::ValueType type;
};

struct TableType : public WasmVM::TableType {
    static std::optional<TableType> get(TokenIter& begin, const TokenIter& end);
};

struct MemType : public WasmVM::MemType {
    static std::optional<MemType> get(TokenIter& begin, const TokenIter& end);
};

struct GlobalType : public WasmVM::GlobalType {
    static std::optional<GlobalType> get(TokenIter& begin, const TokenIter& end);
};

struct Import {
    static std::optional<Import> get(TokenIter& begin, const TokenIter& end);
    std::variant<Type, TableType, MemType, GlobalType> desc;
    std::string module;
    std::string name;
    std::string id;
    Token::Location location;
};

}
}
#endif
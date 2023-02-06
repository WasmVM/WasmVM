#ifndef WASMVM_PP_PARSE_NODES_DEF
#define WASMVM_PP_PARSE_NODES_DEF

#include <Types.hpp>
#include "parse.hpp"
#include <structures/WasmImport.hpp>
#include <structures/WasmInstr.hpp>

#include <map>
#include <type_traits>

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

struct ValueType {
    ValueType(WasmVM::ValueType type) : type(type){}
    static std::optional<ValueType> get(TokenIter& begin, const TokenIter& end);
    operator WasmVM::ValueType();
private:
    WasmVM::ValueType type;
};

using TypeUse = Rule<
    Optional<
        Rule<Token::ParenL, Token::Keyword<"type">, OneOf<Token::Number, Token::Id>, Token::ParenR>
    >,
    Repeat<
        Rule<Token::ParenL, Token::Keyword<"param">, Optional<Token::Id>, Repeat<ValueType>, Token::ParenR>
    >,
    Repeat<
        Rule<Token::ParenL, Token::Keyword<"result">, Repeat<ValueType>, Token::ParenR>
    >
>;

struct Type {
    static std::optional<Type> get(TokenIter& begin, const TokenIter& end);
    Type() = default;
    Type(TypeUse& typeuse);
    std::variant<std::string, index_t> id;
    FuncType func;
    Token::Location location;
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

namespace Instr {

template<typename I, conststr S>
    requires (std::is_base_of<WasmVM::Instr::Base, I>::value)
struct Atomic : public I {
    static std::optional<Atomic<I, S>> get(TokenIter& begin, const TokenIter& end){
        std::list<TokenType>::iterator it = begin;
        if(Token::Keyword<S>::get(it, end)){
            begin = it;
            return Atomic<I, S>();
        }
        return std::nullopt;
    }
};

using Unreachable = Atomic<WasmVM::Instr::Unreachable, "unreachable">;

}

struct Func {
    using Instr = std::variant <
        Instr::Unreachable
    >;
    static std::optional<Func> get(TokenIter& begin, const TokenIter& end);
    Type type;
    std::string id;
    std::vector<ValueType> locals;
    std::map<std::string, index_t> local_id_map;
    std::vector<Instr> body;
    Token::Location location;
};

}
}
#endif
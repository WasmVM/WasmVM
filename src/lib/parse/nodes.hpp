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
    static std::optional<Index> get(TokenIter& begin, const TokenIter& end){
        std::list<TokenType>::iterator it = begin;
        auto syntax = OneOf<Token::Number, Token::Id>::get(it, end);
        if(syntax){
            begin = it;
            return Index(syntax.value());
        }
        return std::nullopt;
    }

    template <typename T>
    T& retrieve(std::vector<T>& container);

    struct Visitor {
        Visitor(std::map<std::string, index_t>& idmap, bool check = true) : idmap(idmap), check(check){}
        std::map<std::string, index_t>& idmap;
        bool check;

        index_t operator()(Token::Number& token){
            return token.unpack<index_t>();
        }
        index_t operator()(Token::Id& token){
            if(token.value.empty()){
                return index_npos;
            }else if(idmap.contains(token.value)){
                return idmap[token.value];
            }else if(check){
                throw Exception::unknown_identifier(token.location, ": index '" + token.value + "' not found");
            }
            return index_npos;
        }
    };

    Token::Location location;
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
    index_t index(WasmModule& module, std::map<std::string, index_t>& typeid_map, std::vector<std::map<std::string, index_t>>& paramid_maps);

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

namespace OneIndex {

struct Base {
    Base(Index index) : index(index){}
    Index index;
};

template<conststr S>
struct Class : public Base {
    Class(const Class&) = default;
    Class(Index& index) : Base(index){}

    static std::optional<Class<S>> get(TokenIter& begin, const TokenIter& end){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<Token::Keyword<S>, Index>::get(it, end);
        if(syntax){
            begin = it;
            return Class<S>(std::get<1>(syntax.value()));
        }
        return std::nullopt;
    }
};

}

using Unreachable = Atomic<WasmVM::Instr::Unreachable, "unreachable">;
using Nop = Atomic<WasmVM::Instr::Nop, "nop">;
struct Block;
struct Loop;
struct If;
using Br = OneIndex::Class<"br">;
using Br_if = OneIndex::Class<"br_if">;
struct Br_table;
using Return = Atomic<WasmVM::Instr::Return, "return">;
using Call = OneIndex::Class<"call">;

using Instrction = std::variant <
    Unreachable, Nop, Block, Loop, If, Br, Br_if, Br_table, Return, Call
>;

struct Block {
    static std::optional<Block> get(TokenIter& begin, const TokenIter& end);
    std::vector<Instrction> instrs;
    std::string id;
    std::optional<std::variant<WasmVM::ValueType, TypeUse>> blocktype;
    Token::Location location;
};

struct Loop {
    static std::optional<Loop> get(TokenIter& begin, const TokenIter& end);
    std::vector<Instrction> instrs;
    std::string id;
    std::optional<std::variant<WasmVM::ValueType, TypeUse>> blocktype;
    Token::Location location;
};

struct If {
    static std::optional<If> get(TokenIter& begin, const TokenIter& end);
    std::vector<Instrction> instrs1, instrs2;
    std::string id;
    std::optional<std::variant<WasmVM::ValueType, TypeUse>> blocktype;
    Token::Location location;
};

struct Br_table {
    static std::optional<Br_table> get(TokenIter& begin, const TokenIter& end);
    std::vector<Index> indices;
};

}

struct Func {
    static std::optional<Func> get(TokenIter& begin, const TokenIter& end);
    Type type;
    std::string id;
    std::vector<ValueType> locals;
    std::map<std::string, index_t> local_id_map;
    std::vector<Instr::Instrction> body;
    Token::Location location;
};

}
}
#endif
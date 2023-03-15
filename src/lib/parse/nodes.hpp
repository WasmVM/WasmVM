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
    static std::optional<Index> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = OneOf<Token::Number, Token::Id>::get(it, holder);
        if(syntax){
            begin = it;
            return Index(syntax.value());
        }
        return std::nullopt;
    }

    template <typename T>
    T& retrieve(std::vector<T>& container);

    struct Visitor {
        Visitor(const std::map<std::string, index_t>& idmap, bool check = true) : idmap(idmap), check(check){}
        const std::map<std::string, index_t>& idmap;
        bool check;

        index_t operator()(Token::Number& token){
            return token.unpack<index_t>();
        }
        index_t operator()(Token::Id& token){
            if(token.value.empty()){
                return index_npos;
            }else if(idmap.contains(token.value)){
                return idmap.at(token.value);
            }else if(check){
                throw Exception::unknown_identifier(token.location, ": index '" + token.value + "' not found");
            }
            return index_npos;
        }
    };

    Token::Location location;
};

struct FuncType : public WasmVM::FuncType {
    static std::optional<FuncType> get(TokenIter& begin, TokenHolder& holder);
    std::map<std::string, index_t> id_map;
};

struct ValueType {
    ValueType(WasmVM::ValueType type) : type(type){}
    static std::optional<ValueType> get(TokenIter& begin, TokenHolder& holder);
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
    static std::optional<Type> get(TokenIter& begin, TokenHolder& holder);
    Type() = default;
    Type(TypeUse& typeuse);
    index_t index(WasmModule& module, std::map<std::string, index_t>& typeid_map, std::vector<std::map<std::string, index_t>>& paramid_maps);

    std::variant<std::string, index_t> id;
    FuncType func;
    Token::Location location;
};

struct TableType : public WasmVM::TableType {
    static std::optional<TableType> get(TokenIter& begin, TokenHolder& holder);
};

struct MemType : public WasmVM::MemType {
    static std::optional<MemType> get(TokenIter& begin, TokenHolder& holder);
};

struct GlobalType : public WasmVM::GlobalType {
    static std::optional<GlobalType> get(TokenIter& begin, TokenHolder& holder);
};

struct Import {
    static std::optional<Import> get(TokenIter& begin, TokenHolder& holder);
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
    static std::optional<Atomic<I, S>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        if(Token::Keyword<S>::get(it, holder)){
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

    static std::optional<Class<S>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<Token::Keyword<S>, Index>::get(it, holder);
        if(syntax){
            begin = it;
            return Class<S>(std::get<1>(syntax.value()));
        }
        return std::nullopt;
    }
};

}

// Control instructions
using Unreachable = Atomic<WasmVM::Instr::Unreachable, "unreachable">;
using Nop = Atomic<WasmVM::Instr::Nop, "nop">;
struct Block;
struct Loop;
struct If;
using Br = OneIndex::Class<"br">;
using Br_if = OneIndex::Class<"br_if">;
struct Br_table {
    static std::optional<Br_table> get(TokenIter& begin, TokenHolder& holder);
    std::vector<Index> indices;
};
using Return = Atomic<WasmVM::Instr::Return, "return">;
using Call = OneIndex::Class<"call">;
struct Call_indirect {
    Call_indirect(std::optional<Index>& tableidx, TypeUse& type) : tableidx(tableidx), type(type) {}
    static std::optional<Call_indirect> get(TokenIter& begin, TokenHolder& holder);
    std::optional<Index> tableidx;
    TypeUse type;
};

// Reference instructions
struct Ref_null : public WasmVM::Instr::Ref_null {
    Ref_null(WasmVM::RefType heaptype) : WasmVM::Instr::Ref_null(heaptype) {}
    static std::optional<Ref_null> get(TokenIter& begin, TokenHolder& holder);
};
using Ref_is_null = Atomic<WasmVM::Instr::Ref_is_null, "ref.is_null">;
using Ref_func = OneIndex::Class<"ref.func">;

// Parametric instructions
using Drop = Atomic<WasmVM::Instr::Drop, "drop">;
struct Select : public WasmVM::Instr::Select {
    static std::optional<Select> get(TokenIter& begin, TokenHolder& holder);
};

// Variable instructions
using Local_get = OneIndex::Class<"local.get">;
using Local_set = OneIndex::Class<"local.set">;
using Local_tee = OneIndex::Class<"local.tee">;
using Global_get = OneIndex::Class<"global.get">;
using Global_set = OneIndex::Class<"global.set">;

// Table instructions
template<conststr S>
struct TableInstr {
    TableInstr(const TableInstr&) = default;
    TableInstr(std::optional<Index>& index): tableidx(index) {}

    static std::optional<TableInstr<S>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<Token::Keyword<S>, Optional<Index>>::get(it, holder);
        if(syntax){
            begin = it;
            return TableInstr<S>(std::get<1>(syntax.value()));
        }
        return std::nullopt;
    }

    std::optional<Index> tableidx;
};

using Table_get = TableInstr<"table.get">;
using Table_set = TableInstr<"table.set">;
using Table_size = TableInstr<"table.size">; 
using Table_grow = TableInstr<"table.grow">; 
using Table_fill = TableInstr<"table.fill">;
struct Table_copy {
    Table_copy(const Table_copy&) = default;
    Table_copy(std::optional<Index> dstidx, std::optional<Index> srcidx): dstidx(dstidx), srcidx(srcidx) {}
    static std::optional<Table_copy> get(TokenIter& begin, TokenHolder& holder);
    
    std::optional<Index> dstidx;
    std::optional<Index> srcidx;
};
struct Table_init {
    Table_init(const Table_init&) = default;
    Table_init(std::optional<Index> tableidx, Index& elemidx): tableidx(tableidx), elemidx(elemidx) {}
    static std::optional<Table_init> get(TokenIter& begin, TokenHolder& holder);
    
    std::optional<Index> tableidx;
    Index elemidx;
};
using Elem_drop = OneIndex::Class<"elem.drop">;

// Memory Instructions
template<typename I, conststr S, align_t N>
    requires (std::is_base_of<WasmVM::Instr::MemoryInstr::Base, I>::value)
struct MemoryInstr : public I {

    MemoryInstr(const MemoryInstr&) = default;
    MemoryInstr() : I(0, 0, N) {}

    static std::optional<MemoryInstr<I, S, N>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<
            Token::Keyword<S>,
            Optional<Token::Number>,
            Optional<Token::MemArg<"offset">>,
            Optional<Token::MemArg<"align">>
        >::get(it, holder);
        if(syntax){
            MemoryInstr<I, S, N> instr;
            auto rule = syntax.value();
            auto memidx = std::get<1>(rule);
            if(memidx){
                Token::Number value = memidx.value();
                instr.memidx = value.unpack<u32_t>();
            }
            auto offset = std::get<2>(rule);
            if(offset){
                Token::MemArgBase value = offset.value();
                instr.offset = value.unpack<u64_t>();
            }
            auto align = std::get<3>(rule);
            if(align){
                Token::MemArgBase value = align.value();
                instr.align = value.unpack<u32_t>();
            }
            begin = it;
            return instr;
        }
        return std::nullopt;
    }
};

using I32_load = MemoryInstr<WasmVM::Instr::I32_load, "i32.load", sizeof(i32_t)>;
using I64_load = MemoryInstr<WasmVM::Instr::I64_load, "i64.load", sizeof(i64_t)>;
using F32_load = MemoryInstr<WasmVM::Instr::F32_load, "f32.load", sizeof(f32_t)>;
using F64_load = MemoryInstr<WasmVM::Instr::F64_load, "f64.load", sizeof(f64_t)>;
using I32_load8_s = MemoryInstr<WasmVM::Instr::I32_load8_s, "i32.load8_s", sizeof(i8_t)>;
using I32_load8_u = MemoryInstr<WasmVM::Instr::I32_load8_u, "i32.load8_u", sizeof(u8_t)>;
using I32_load16_s = MemoryInstr<WasmVM::Instr::I32_load16_s, "i32.load16_s", sizeof(i16_t)>;
using I32_load16_u = MemoryInstr<WasmVM::Instr::I32_load16_u, "i32.load16_u", sizeof(u16_t)>;
using I64_load8_s = MemoryInstr<WasmVM::Instr::I64_load8_s, "i64.load8_s", sizeof(i8_t)>;
using I64_load8_u = MemoryInstr<WasmVM::Instr::I64_load8_u, "i64.load8_u", sizeof(u8_t)>;
using I64_load16_s = MemoryInstr<WasmVM::Instr::I64_load16_s, "i64.load16_s", sizeof(i16_t)>;
using I64_load16_u = MemoryInstr<WasmVM::Instr::I64_load16_u, "i64.load16_u", sizeof(u16_t)>;
using I64_load32_s = MemoryInstr<WasmVM::Instr::I64_load32_s, "i64.load32_s", sizeof(i32_t)>;
using I64_load32_u = MemoryInstr<WasmVM::Instr::I64_load32_u, "i64.load32_u", sizeof(u32_t)>;
using I32_store = MemoryInstr<WasmVM::Instr::I32_store, "i32.store", sizeof(i32_t)>;
using I64_store = MemoryInstr<WasmVM::Instr::I64_store, "i64.store", sizeof(i64_t)>;
using F32_store = MemoryInstr<WasmVM::Instr::F32_store, "f32.store", sizeof(f32_t)>;
using F64_store = MemoryInstr<WasmVM::Instr::F64_store, "f64.store", sizeof(f64_t)>;
using I32_store8 = MemoryInstr<WasmVM::Instr::I32_store8, "i32.store8", sizeof(i8_t)>;
using I32_store16 = MemoryInstr<WasmVM::Instr::I32_store16, "i32.store16", sizeof(i16_t)>;
using I64_store8 = MemoryInstr<WasmVM::Instr::I64_store8, "i64.store8", sizeof(i8_t)>;
using I64_store16 = MemoryInstr<WasmVM::Instr::I64_store16, "i64.store16", sizeof(i16_t)>;
using I64_store32 = MemoryInstr<WasmVM::Instr::I64_store32, "i64.store32", sizeof(i32_t)>;

template<conststr S>
struct MemoryOneIndex {
    MemoryOneIndex(const MemoryOneIndex&) = default;
    MemoryOneIndex(std::optional<Index>& index): memidx(index) {}

    static std::optional<MemoryOneIndex<S>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<Token::Keyword<S>, Optional<Index>>::get(it, holder);
        if(syntax){
            begin = it;
            return MemoryOneIndex<S>(std::get<1>(syntax.value()));
        }
        return std::nullopt;
    }

    std::optional<Index> memidx;
};

using Memory_size = MemoryOneIndex<"memory.size">;
using Memory_grow = MemoryOneIndex<"memory.grow">;
using Memory_fill = MemoryOneIndex<"memory.fill">;
struct Memory_copy {
    Memory_copy(const Memory_copy&) = default;
    Memory_copy(std::optional<Index> dstidx, std::optional<Index> srcidx): dstidx(dstidx), srcidx(srcidx) {}
    static std::optional<Memory_copy> get(TokenIter& begin, TokenHolder& holder);
    
    std::optional<Index> dstidx;
    std::optional<Index> srcidx;
};
struct Memory_init {
    Memory_init(const Memory_init&) = default;
    Memory_init(std::optional<Index> memidx, Index& dataidx): memidx(memidx), dataidx(dataidx) {}
    static std::optional<Memory_init> get(TokenIter& begin, TokenHolder& holder);
    
    std::optional<Index> memidx;
    Index dataidx;
};
using Data_drop = OneIndex::Class<"data.drop">;

// Numeric instructions
template<typename I, conststr S, typename T>
struct NumericConst : public I {
    NumericConst(const NumericConst&) = default;
    NumericConst(Token::Number value): I(value.unpack<T>()) {}
    static std::optional<NumericConst<I, S, T>> get(TokenIter& begin, TokenHolder& holder){
        std::list<TokenType>::iterator it = begin;
        auto syntax = Rule<Token::Keyword<S>, Token::Number>::get(it, holder);
        if(syntax){
            begin = it;
            return NumericConst<I, S, T>(std::get<1>(syntax.value()));
        }
        return std::nullopt;
    }
};
using I32_const = NumericConst<WasmVM::Instr::I32_const, "i32.const", i32_t>;
using I64_const = NumericConst<WasmVM::Instr::I64_const, "i64.const", i64_t>;
using F32_const = NumericConst<WasmVM::Instr::F32_const, "f32.const", f32_t>;
using F64_const = NumericConst<WasmVM::Instr::F64_const, "f64.const", f64_t>;

// Instruction types
using Instrction = std::variant <
    Unreachable, Nop, Block, Loop, If, Br, Br_if, Br_table, Return, Call, Call_indirect,
    Ref_null, Ref_is_null, Ref_func,
    Drop, Select,
    Local_get, Local_set, Local_tee, Global_get, Global_set,
    Table_get, Table_set, Table_size, Table_grow, Table_fill, Table_copy, Table_init, Elem_drop,
    I32_load, I64_load, F32_load, F64_load, I32_load8_s, I32_load8_u, I32_load16_s, I32_load16_u,
    I64_load8_s, I64_load8_u, I64_load16_s, I64_load16_u, I64_load32_s, I64_load32_u,
    I32_store, I64_store, F32_store, F64_store, I32_store8, I32_store16, I64_store8, I64_store16, I64_store32,
    Memory_size, Memory_grow, Memory_fill, Memory_copy, Memory_init, Data_drop,
    I32_const, I64_const, F32_const, F64_const
>;

// Block instructions
struct Block {
    static std::optional<Block> get(TokenIter& begin, TokenHolder& holder);
    std::vector<Instrction> instrs;
    std::string id;
    std::optional<TypeUse> blocktype;
    Token::Location location;
};

struct Loop {
    static std::optional<Loop> get(TokenIter& begin, TokenHolder& holder);
    std::vector<Instrction> instrs;
    std::string id;
    std::optional<TypeUse> blocktype;
    Token::Location location;
};

struct If {
    static std::optional<If> get(TokenIter& begin, TokenHolder& holder);
    std::vector<Instrction> thenInstrs, elseInstrs;
    std::vector<Instrction> foldInstrs;
    std::string id;
    std::optional<TypeUse> blocktype;
    Token::Location location;
};

}

struct Func {
    static std::optional<Func> get(TokenIter& begin, TokenHolder& holder);
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
#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

enum ValueType : u8_t {
    i32 = 0x7f, i64 = 0x7e, f32 = 0x7d, f64 = 0x7c, funcref = 0x70, externref = 0x6f
};

// Convert internal WasmVM::ValueType to Objdump raw byte ValueType
inline ValueType to_objdump_valtype(::WasmVM::ValueType vt){
    switch(vt){
        case ::WasmVM::ValueType::i32: return ValueType::i32;
        case ::WasmVM::ValueType::i64: return ValueType::i64;
        case ::WasmVM::ValueType::f32: return ValueType::f32;
        case ::WasmVM::ValueType::f64: return ValueType::f64;
        case ::WasmVM::ValueType::funcref: return ValueType::funcref;
        case ::WasmVM::ValueType::externref: return ValueType::externref;
        default: return ValueType::i32;
    }
}

// Convert raw Objdump::ValueType (byte codes) to WasmVM::ValueType (internal enum)
inline ::WasmVM::ValueType to_wasm_valtype(ValueType vt){
    switch(vt){
        case ValueType::i32: return ::WasmVM::ValueType::i32;
        case ValueType::i64: return ::WasmVM::ValueType::i64;
        case ValueType::f32: return ::WasmVM::ValueType::f32;
        case ValueType::f64: return ::WasmVM::ValueType::f64;
        case ValueType::funcref: return ::WasmVM::ValueType::funcref;
        case ValueType::externref: return ::WasmVM::ValueType::externref;
        default: return ::WasmVM::ValueType::i32;
    }
}

struct Bytes : public std::vector<byte_t> {
    Bytes() : std::vector<byte_t>() {}
    Bytes(size_t size) : std::vector<byte_t>(size) {}
    std::istream::pos_type address;
};

// Dumper class: handles all formatting and output logic (separated from parsing)
struct Dumper {
    Dumper(std::istream& istream);
    
    void print_address(std::ostream& os, const Bytes& bytes) const;
    void print_address(std::ostream& os, long address) const;
    void print_hex(std::ostream& os, const Bytes& bytes) const;
    
    size_t get_address_width() const { return address_width; }
    
private:
    size_t address_width;
};

struct Section; // pre declare in stream

struct Stream {
    Stream(std::istream& istream);

    template <typename T> // T can be any type
    friend Stream& operator>>(Stream&, T&);
    template <typename T>
    friend Stream& operator>>(Stream&, std::vector<T>&);
    friend std::ostream& operator<<(std::ostream&, Stream&);
    friend std::ostream& operator<<(std::ostream&, Bytes&);
    friend std::ostream& operator<<(std::ostream&, Section&);
    template <typename T>
    friend std::ostream& operator<<(std::ostream&, std::vector<T>&);

    u32_t peek_section(Stream&);

    Bytes get_u32(Stream&);
    Bytes get_u64(Stream&);
    Bytes get_f32(Stream&);
    Bytes get_f64(Stream&);
    u32_t to_u32(Bytes&);
    u64_t to_u64(Bytes&);
    
    std::istream& get_istream() { return istream; }
    
private:
    std::istream& istream;
};

struct Section {
    Section(Stream& stream) : stream(stream) {}
    Stream& stream;
    Bytes   id = Bytes(1);                  // Raw section ID byte
    Bytes   size;                           // Raw LEB128 encoded size
    Bytes   raw_count = Bytes();            // Raw LEB128 encoded count
    std::istream::pos_type size_address;    // Stream position of size field
};

struct TypeSection : public Section {
    TypeSection(Stream& stream) : Section(stream) {}
    std::vector<FuncType> functype;
};

struct ImportSection : public Section {
    ImportSection(Stream& stream) : Section(stream) {}
    std::vector<WasmImport> imports;
    std::vector<Bytes> raw_imports;         // Store raw bytes for each import
};

struct FunctionSection : public Section {
    FunctionSection(Stream& stream) : Section(stream) {}
    std::vector<index_t> functions;
    std::vector<Bytes> raw_functions;       // Store raw LEB128 bytes for each function index
};

struct TableSection : public Section {
    TableSection(Stream& stream) : Section(stream) {}
    std::vector<TableType> tables;
    std::vector<Bytes> raw_tables;          // Store raw bytes for each table
};

struct MemorySection : public Section {
    MemorySection(Stream& stream) : Section(stream) {}
    std::vector<MemType> memories;
    std::vector<Bytes> raw_memories;        // Store raw bytes for each memory
};

struct GlobalSection : public Section {
    GlobalSection(Stream& stream) : Section(stream) {}
    std::vector<WasmGlobal> globals;
    std::vector<Bytes> raw_globals;         // Store raw bytes for each global
};

struct ExportSection : public Section {
    ExportSection(Stream& stream) : Section(stream) {}
    std::vector<WasmGlobal> exports;
    std::vector<Bytes> raw_exports;         // Store raw bytes for each export
};

template <typename T>
Stream& operator>>(Stream&, T&);

template <typename T>
Stream& operator>>(Stream& stream, std::vector<T>& vec) {
    Bytes tempsize;
    // Read data size
    tempsize = stream.get_u32(stream);
    u32_t nums = 0;
    for (int i = 0; i < tempsize.size(); i++){
        nums |= ((int)tempsize[i] & 0x7F) << (i*7);
    }
    vec.resize(nums);
    for (int i = 0; i < nums; ++i) {
        // add raw data to data structure
        stream >> vec[i];
    }
    return stream;
}

std::ostream& operator<<(std::ostream&, Stream&);
std::ostream& operator<<(std::ostream&, Bytes&);
std::ostream& operator<<(std::ostream&, Section&);
std::ostream& operator<<(std::ostream&, TypeSection&);
std::ostream& operator<<(std::ostream&, ValueType&);
std::ostream& operator<<(std::ostream&, ImportSection&);
std::ostream& operator<<(std::ostream&, FunctionSection&);
std::ostream& operator<<(std::ostream&, TableSection&);
std::ostream& operator<<(std::ostream&, MemorySection&);
std::ostream& operator<<(std::ostream&, GlobalSection&);



} // namespace Objdump
} // namespace WasmVM

#endif

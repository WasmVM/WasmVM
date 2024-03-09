#ifndef WASMVM_PP_DECODE_DEF
#define WASMVM_PP_DECODE_DEF

#include <WasmVM.hpp>
#include <istream>
#include <optional>
#include <unordered_map>

namespace WasmVM {
namespace Decode {

struct Stream {
    Stream(std::istream& istream, WasmModule& module) : istream(istream), module(module){}

    template <typename T>
    friend Stream& operator>>(Stream& stream, T& value);

    template <typename T>
    T get(){
        T value;
        *this >> value;
        return value;
    }

    template <typename T>
    friend Stream& operator>>(Stream& stream, std::vector<T>& values){
        u32_t size;
        for(stream >> size; size > 0; --size){
            stream >> values.emplace_back();
        }
        return stream;
    }
    friend class Section;
    
    byte_t peek();
    size_t location();
private:
    std::istream& istream;
    WasmModule& module;
    std::unordered_map<ValueType, index_t> typemap;
};

template <typename T>
Stream& operator>>(Stream& stream, T& value);

struct Section {
    friend Stream& operator>>(Stream& stream, Section&& secion);
    Section(const byte_t id = (byte_t)0x00) : id(id) {};
    const byte_t id;
protected:
    virtual Stream& read(Stream& stream);
    std::optional<size_t> header(Stream& stream);
};
Stream& operator>>(Stream& stream, Section&& secion);

struct Type : public Section {
    Type(std::vector<FuncType>& types) : Section((byte_t)0x01), types(types){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<FuncType>& types;
};

struct Import : public Section {
    Import(std::vector<WasmImport>& imports) : Section((byte_t)0x02), imports(imports){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmImport>& imports;
};

struct Func : public Section {
    Func(std::vector<WasmFunc>& funcs) : Section((byte_t)0x03), funcs(funcs){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmFunc>& funcs;
};

struct Table : public Section {
    Table(std::vector<TableType>& tables) : Section((byte_t)0x04), tables(tables){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<TableType>& tables;
};

struct Memory : public Section {
    Memory(std::vector<MemType>& mems) : Section((byte_t)0x05), mems(mems){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<MemType>& mems;
};

struct Global : public Section {
    Global(std::vector<WasmGlobal>& globals) : Section((byte_t)0x06), globals(globals){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmGlobal>& globals;
};

struct Export : public Section {
    Export(std::vector<WasmExport>& exports) : Section((byte_t)0x07), exports(exports){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmExport>& exports;
};

struct Start : public Section {
    Start(std::optional<index_t>& start) : Section((byte_t)0x08), start(start){}
protected:
    Stream& read(Stream& stream) override;
    std::optional<index_t>& start;
};

struct Elem : public Section {
    Elem(std::vector<WasmElem>& elems) : Section((byte_t)0x09), elems(elems){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmElem>& elems;
};

struct Code : public Section {
    Code(std::vector<WasmFunc>& funcs) : Section((byte_t)0x0a), funcs(funcs){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmFunc>& funcs;
};

struct Data : public Section {
    Data(std::vector<WasmData>& datas) : Section((byte_t)0x0b), datas(datas){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmData>& datas;
};

struct DataCount : public Section {
    DataCount(std::vector<WasmData>& datas) : Section((byte_t)0x0c), datas(datas){}
protected:
    Stream& read(Stream& stream) override;
    std::vector<WasmData>& datas;
};

} // namespace Decode
} // namespace WasmVM

#endif
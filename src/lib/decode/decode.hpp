#ifndef WASMVM_PP_DECODE_DEF
#define WASMVM_PP_DECODE_DEF

#include <WasmVM.hpp>
#include <istream>
#include <optional>

namespace WasmVM {
namespace Decode {

constexpr u32_t magic = 0x6d736100;
constexpr u32_t version = 0x01;

struct Stream {
    Stream(std::istream& istream) : istream(istream){}

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

} // namespace Decode
} // namespace WasmVM

#endif
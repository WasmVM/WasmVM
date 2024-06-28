#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

struct Bytes : public std::vector<byte_t> {
    Bytes() : std::vector<byte_t>() {}
    Bytes(size_t size) : std::vector<byte_t>(size) {}
    std::istream::pos_type address;
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

    void print_address(Bytes&);
    void print_address(size_t&);
    Bytes get_u32(Stream&);
    
private:
    std::istream& istream;
    size_t address_width;
};

struct Section {
    Section(Stream& stream) : stream(stream) {}
    Stream& stream;
    Bytes   id = Bytes(1);
    Bytes   size;
    size_t  size_address;
};

struct TypeSection : public Section {
    TypeSection(Stream& stream) : Section(stream) {}
    std::vector<FuncType> functype;
};

template <typename T>
Stream& operator>>(Stream&, T&);

template <typename T>
Stream& operator>>(Stream& stream, std::vector<T>& vec) {
    Bytes tempsize;
    tempsize = stream.get_u32(stream);
    u32_t nums = 0;
    for (int i = 0; i < tempsize.size(); i++){
        nums |= ((int)tempsize[i] & 0x7F) << (i*7);
    }
    vec.resize(nums);
    for (int i = 0; i < nums; ++i) {
        stream >> vec[i];
    }
    return stream;
}

std::ostream& operator<<(std::ostream&, Stream&);
std::ostream& operator<<(std::ostream&, Bytes&);
std::ostream& operator<<(std::ostream&, Section&);
std::ostream& operator<<(std::ostream&, TypeSection&);
std::ostream& operator<<(std::ostream&, ValueType&);

} // namespace Objdump
} // namespace WasmVM

#endif
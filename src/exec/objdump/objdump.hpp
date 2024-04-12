#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

struct Bytes : public std::vector<byte_t> {
    Bytes(size_t size) : std::vector<byte_t>(size) {}
    std::istream::pos_type address;
};

struct Section;

struct Stream {
    Stream(std::istream& istream);

    template <typename T> // T can be any type
    friend Stream& operator>>(Stream&, T&);

    friend std::ostream& operator<<(std::ostream&, Stream&);
    friend std::ostream& operator<<(std::ostream&, Bytes&);
    friend std::ostream& operator<<(std::ostream&, Section&);

    void print_address(Bytes&);
    void print_address(size_t&);
    
private:
    std::istream& istream;
    size_t address_width;
};

struct Section {
    Section(Stream& stream) : stream(stream) {}
    Bytes id = Bytes(1);
    u32_t size;
    size_t size_address;
    Stream& stream;
};

template <typename T>
Stream& operator>>(Stream&, T&);

std::ostream& operator<<(std::ostream&, Stream&);
std::ostream& operator<<(std::ostream&, Bytes&);
std::ostream& operator<<(std::ostream&, Section&);

} // namespace Objdump
} // namespace WasmVM

#endif
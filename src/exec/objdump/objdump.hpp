#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

struct Byte {
    std::istream::pos_type address;
    std::vector<byte_t> byte;
};

struct Stream {
    Stream(std::istream& istream) : istream(istream){}

    template <typename T> // T can be any type
    friend Stream& operator>>(Stream&, T&);

    friend std::ostream& operator<<(std::ostream&, Stream&);
    friend std::ostream& operator<<(std::ostream&, Byte&);
    
private:
    std::istream& istream;
};

template <typename T>
Stream& operator>>(Stream&, T&);

std::ostream& operator<<(std::ostream&, Stream&);
std::ostream& operator<<(std::ostream&, Byte&);

void printBytes(int count);



} // namespace Objdump
} // namespace WasmVM

#endif
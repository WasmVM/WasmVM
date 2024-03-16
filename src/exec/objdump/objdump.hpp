#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

struct Byte {
    std::istream::pos_type address;
    byte_t byte;
};

struct Stream {
    Stream(std::istream& istream) : istream(istream){}

    template <typename T>
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



} // namespace Objdump
} // namespace WasmVM

#endif
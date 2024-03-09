#ifndef WASMVM_EXEC_Objdump_DEF
#define WASMVM_EXEC_Objdump_DEF

#include <WasmVM.hpp>
#include <iostream>

namespace WasmVM {
namespace Objdump {

struct Stream {
    Stream(std::istream& istream) : istream(istream){}

    template <typename T>
    friend Stream& operator>>(Stream&, T&);
    
private:
    std::istream& istream;
};

template <typename T>
Stream& operator>>(Stream&, T&);

} // namespace Objdump
} // namespace WasmVM

#endif
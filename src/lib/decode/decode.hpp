#ifndef WASMVM_PP_DECODE_DEF
#define WASMVM_PP_DECODE_DEF

#include <WasmVM.hpp>
#include <istream>

namespace WasmVM {
namespace Decode {

constexpr u32_t magic = 0x6d736100;
constexpr u32_t version = 0x01;

class Section {
public:
    void read(std::istream& stream);
protected:
    Section(const byte_t id, WasmModule& module);
    const byte_t id;
    WasmModule& module;

    struct Stream {
        Stream(std::istream& stream);

        template <typename T>
        Stream& operator>>(T value);

        template <typename T>
        Stream& operator>>(const std::vector<T> values){
            u32_t size;
            for(*this >> size; size > 0; --size){
                T value;
                *this >> value;
                values.emplace_back(value);
            }
            return *this;
        }
    private:
        std::istream& stream;
    };
};

} // namespace Decode
} // namespace WasmVM

#endif
#ifndef WASMVM_PP_ENCODE_DEF
#define WASMVM_PP_ENCODE_DEF

#include <WasmVM.hpp>
#include <ostream>
#include <sstream>
#include <vector>
#include <cstddef>

namespace WasmVM {

namespace Encode {

constexpr char magic[] = "\x00\x61\x73\x6d\x01\x00\x00\x00";

class Section {
public:
    void write(std::ostream& stream);
protected:
    Section(const byte_t id);
    const byte_t id;
    std::stringstream buffer;

    struct Stream {
        Stream(std::ostream& stream);

        template <typename T>
        Stream& operator<<(T value);

        template <typename T>
        Stream& operator<<(const std::vector<T> values){
            *this << (u32_t)values.size();
            for(T value : values){
                *this << value;
            }
            return *this;
        }

        Stream& operator<<(std::stringstream& value);
    private:
        std::ostream& stream;
    };
};

struct Type : public Section {
    Type(const std::vector<FuncType>& types);
};

struct Import : public Section {
    Import(const std::vector<WasmImport>& imports);
};

} // namespace Encode
} // namespace WasmVM

#endif
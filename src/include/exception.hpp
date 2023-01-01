#ifndef WASMVM_EXCEPTION_DEF
#define WASMVM_EXCEPTION_DEF

#include <exception>
#include <utility>

#include <Util.hpp>

#define define_exception(NAME, MSG) \
    struct NAME : Exception<MSG>{NAME(std::pair<size_t, size_t> loc) : Exception<MSG>(loc){}};

namespace WasmVM {
namespace Exception{

    template <conststr M>
    struct Exception : std::runtime_error {
        Exception(std::pair<size_t, size_t> loc) : std::runtime_error(M.value), location(loc){}
        std::pair<size_t, size_t> location;
    };

    define_exception(string_not_close, "string not close")
    define_exception(block_comment_not_close, "block comment not close");
}
}

#undef define_exception

#endif
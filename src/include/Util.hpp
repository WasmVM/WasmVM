#ifndef WASMVM_PP_UTIL_DEF
#define WASMVM_PP_UTIL_DEF

#include <algorithm>
#include <functional>

template <size_t N>
struct conststr {
    constexpr conststr(const char (&arg)[N]) {
        std::copy_n(arg, N, value);
    }
    template<size_t M>
    constexpr bool operator==(const conststr<M>& str) const {
        return (N == M) && std::equal(value, value + N, str.value);
    };
    template<size_t M>
    constexpr bool operator!=(const conststr<M>& str) const {
        return !(*this == str);
    };
    char value[N];
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif
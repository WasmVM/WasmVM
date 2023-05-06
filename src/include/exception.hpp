#ifndef WASMVM_EXCEPTION_DEF
#define WASMVM_EXCEPTION_DEF

#include <ostream>
#include <exception>
#include <optional>
#include <functional>
#include <utility>

#include <Util.hpp>

namespace WasmVM {
namespace Exception{

    struct Exception : public std::runtime_error {
        Exception(std::string msg) : std::runtime_error(msg) {}
    };

    struct Parse : public Exception {
        Parse(std::string msg, std::pair<size_t, size_t> location) : Exception(msg), location(location) {}
        std::pair<size_t, size_t> location;
    };
    struct Decode : public Exception {
        Decode(std::string msg, size_t location) : Exception(msg), location(location) {}
        size_t location;
    };

    struct Warning {
        Warning(std::string msg){
            if(handler){
                handler->operator()(msg);
            }
        }
        Warning(const char* msg){
            Warning(std::string(msg));
        }
        static void regist(std::function<void(std::string)> handler){
            Warning::handler.emplace(handler);
        };
    private:
        static std::optional<std::function<void(std::string)>> handler;
    };
}

}

#undef define_exception

#endif
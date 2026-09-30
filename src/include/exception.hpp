#ifndef WASMVM_EXCEPTION_DEF
#define WASMVM_EXCEPTION_DEF

#include <ostream>
#include <exception>
#include <optional>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <Types.hpp>
#include <Util.hpp>

namespace WasmVM {
namespace Exception{

    // Every exception type here and in the library declares its destructor
    // out of line. That destructor is the class's key function, so its
    // typeinfo is emitted once, in the library, and every other image refers
    // to that one copy. Without it each image carries its own weak copy; an
    // LTO-built executable hides its copy, and on x86-64, where the runtime
    // compares typeinfo by address, a `catch` there no longer matches what
    // the library throws. A new exception type needs the same.
    struct Exception : public std::runtime_error {
        Exception(std::string msg) : std::runtime_error(msg) {}
        ~Exception() override;
    };

    struct Parse : public Exception {
        Parse(std::string msg, std::pair<size_t, size_t> location) : Exception(msg), location(location) {}
        ~Parse() override;
        std::pair<size_t, size_t> location;
    };
    struct Decode : public Exception {
        Decode(std::string msg, size_t location) : Exception(msg), location(location) {}
        ~Decode() override;
        size_t location;
    };
    struct unexpected_eof : public Exception {
        unexpected_eof();
        ~unexpected_eof() override;
    };

    // Thrown by Stack::run() when the embedder's interrupt flag (see Stack)
    // reads true. Deliberately not a wasm-level exception, so the guest's
    // try_table/catch_all can never swallow it.
    struct interrupted : public Exception {
        interrupted();
        ~interrupted() override;
    };

    // Base of every WebAssembly trap escaping Stack::run(): the guest hit a
    // condition the spec defines as trapping (unreachable, integer divide by
    // zero, out-of-bounds memory/table access, indirect call type mismatch,
    // null reference, failed cast, ...). The concrete types are private to
    // WasmVM; embedders catch this base.
    struct Trap : public Exception {
        Trap(std::string msg) : Exception(msg) {}
        ~Trap() override;
    };

    // A wasm `throw` no try_table in the guest caught, escaping Stack::run().
    // Not a Trap: the spec keeps exceptions and traps distinct. Carries the
    // thrown exception (tag address and payload values).
    struct Uncaught : public Exception {
        Uncaught(GcPtr<WasmException> exn);
        ~Uncaught() override;
        GcPtr<WasmException> exn;
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
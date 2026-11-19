#ifndef WASMVM_EXEC_DAP_FRAMING_DEF
#define WASMVM_EXEC_DAP_FRAMING_DEF

// DAP base protocol framing: each message is "Content-Length: N\r\n\r\n"
// followed by N bytes of JSON. Pure functions on strings, no I/O.

#include <cstddef>
#include <optional>
#include <string>

namespace Dap {

/// Collects bytes as they arrive and splits them into message bodies.
class MessageReader {
public:
    void feed(const char* data, size_t size);
    /// The next complete body, or nullopt until more bytes arrive.
    /// Throws WasmVM::Exception::Exception on a malformed header.
    std::optional<std::string> next();
private:
    std::string buffer;
};

/// Prepends the Content-Length header to a JSON body.
std::string frame(const std::string& body);

}

#endif

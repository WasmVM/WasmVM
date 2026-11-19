#ifndef WASMVM_EXEC_DAP_SOCKET_DEF
#define WASMVM_EXEC_DAP_SOCKET_DEF

// TCP sockets for the DAP server (#54): POSIX and Winsock behind one small
// interface, with no third-party dependency. Only what the server needs: listen
// on one address, accept one connection, read, write, shut down.

#include <cstddef>
#include <cstdint>
#include <string>

namespace Dap {

/// `--dap` argument: "port", "host:port" or "[ipv6]:port". The host defaults
/// to 127.0.0.1. Throws WasmVM::Exception::Exception on a malformed spec.
struct Endpoint {
    std::string host;
    uint16_t port;
    static Endpoint parse(const std::string& spec);
};

#ifdef _WIN32
using socket_handle = uintptr_t; // SOCKET
#else
using socket_handle = int;
#endif

class Connection {
public:
    Connection() = default;
    explicit Connection(socket_handle handle) : handle(handle), valid(true){}
    Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&) noexcept;
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    ~Connection();

    /// Reads at most `size` bytes; returns 0 when the peer closed or the
    /// connection was shut down, never throws.
    size_t read(char* buffer, size_t size);
    /// Writes everything; returns false when the connection is gone.
    bool write(const std::string& data);
    /// Ends both directions; a read blocked on another thread returns 0.
    void shutdown();
    void close();
private:
    socket_handle handle = 0;
    bool valid = false;
};

class Listener {
public:
    /// Binds and listens; throws WasmVM::Exception::Exception on failure.
    explicit Listener(const Endpoint& endpoint);
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;
    ~Listener();
    /// The bound address as "host:port" ("[host]:port" for IPv6), with the
    /// actual port when 0 was asked for.
    std::string address() const;
    /// Blocks for one connection.
    Connection accept();
    void close();
private:
    socket_handle handle = 0;
    bool valid = false;
};

}

#endif

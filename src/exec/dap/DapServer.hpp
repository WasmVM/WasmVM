#ifndef WASMVM_EXEC_DAP_SERVER_DEF
#define WASMVM_EXEC_DAP_SERVER_DEF

// Debugger Layer 2 (#54): a Debug Adapter Protocol server for `wasmvm --dap`.
//
// Threads: the VM runs on the thread that calls module_instanciate (wasmvm's
// main thread); Layer 1 stops it by calling on_stop() there, which blocks until
// the client resumes. This class owns one more thread, which reads requests
// from the socket and answers them. While the VM is stopped, that thread reads
// the stopped Stack and the Store; while it runs, requests that need them are
// refused.
//
// Sources: one virtual WAT source per defined function of every module the
// command line loads, built before the session starts and never changed, served
// by sourceReference. Line 1 is the function header; line k + 2 is instruction
// k of WasmFunc::body, the trailing `end` last.

#include <Debugger.hpp>
#include <WasmVM.hpp>
#include <json.hpp>
#include <instances/Store.hpp>
#include <instances/Stack.hpp>

#include <condition_variable>
#include <filesystem>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "Socket.hpp"

namespace Dap {

class Server : public WasmVM::Debugger {
public:
    /// Thrown from on_stop() (and before_instantiate()) once the client asked
    /// to terminate the debuggee; wasmvm then exits.
    struct Terminate {};

    Server(Connection connection, WasmVM::Store& store);
    ~Server();

    /// Registers a module, in instantiation order, before start(). `module`
    /// must outlive the server.
    void add_module(const std::filesystem::path& path, const WasmVM::WasmModule& module);
    /// Starts the request thread.
    void start();
    /// Blocks until the client sent launch and configurationDone; false when
    /// it went away first.
    bool wait_configured();
    /// Around module_instanciate(moduleinst, ..., this) for module `index`.
    void before_instantiate(size_t index, WasmVM::ModuleInst& moduleinst);
    void after_instantiate(size_t index);
    /// The program ended: sends exited and terminated, then waits until the
    /// client disconnects.
    void finish(int exit_code);

    Resume on_stop(const Stop& stop) override;

    // Exposed for tests.
    struct Source {
        size_t module;              // index into modules
        WasmVM::index_t funcidx;    // in the module's function index space
        std::string name;           // "file.wasm:$f" or "file.wasm:func N"
        std::string text;           // the virtual WAT
        std::vector<long> opener;   // per instruction: for an `end`, the index of the
                                    // block/loop/if/try_table it closes, -1 for the
                                    // function's own; -1 for anything else
        std::optional<WasmVM::index_t> funcaddr; // once instantiated
        std::set<WasmVM::index_t> breakpoints;   // instruction indices
    };
    static Source make_source(const WasmVM::WasmModule& module, WasmVM::index_t funcidx, const std::string& file);
    static std::string format_value(const WasmVM::Value& value);
    static std::string value_type(const WasmVM::Value& value);

private:
    struct Module {
        std::filesystem::path path;
        const WasmVM::WasmModule* module;
        WasmVM::index_t import_funcs = 0;
        std::vector<bool> global_mutable; // global index space
        WasmVM::ModuleInst* inst = nullptr;
        bool bound = false;
        std::vector<size_t> sources; // per defined function
    };
    struct VarRef {
        enum Kind { Locals, Operands, Label, Globals, Memory } kind;
        size_t frame;
        size_t label = 0;
    };

    // Request thread.
    void serve();
    void handle(Json::Value& request);
    void send(Json::Value::Object message);
    void respond(Json::Value& request, Json::Value body = Json::Value());
    void fail(Json::Value& request, const std::string& message);
    void event(const std::string& name, Json::Value body = Json::Value());
    void disconnect(bool terminate);

    // Guarded by `mutex`.
    void bind(size_t index);
    void apply_breakpoints(Source& source, const std::set<WasmVM::index_t>& previous);
    std::optional<size_t> source_of(WasmVM::index_t funcaddr) const;
    const Module* module_of(const WasmVM::ModuleInst& inst) const;
    std::vector<const WasmVM::Frame*> frames() const;
    long frame_instr(size_t frame) const;
    Json::Value stack_trace();
    Json::Value scopes(size_t frame);
    Json::Value variables(const VarRef& ref);
    Resume wait_for_resume(const Stop& stop, const std::string& reason, const std::string& text);

    Connection connection;
    WasmVM::Store& store;
    std::vector<Module> modules;
    std::vector<Source> sources;
    std::thread thread;

    std::mutex write_mutex;
    long long seq = 1;

    std::mutex mutex;
    std::condition_variable changed;
    bool launched = false;
    bool configured = false;
    bool connected = true;   // the request thread still runs
    bool detached = false;   // disconnected with terminateDebuggee false
    bool terminating = false;
    bool stop_on_entry = false;
    bool entry_reported = false;
    bool stop_on_trap = true;
    bool stop_on_uncaught = true;
    bool lines_start_at_1 = true;
    bool pause_requested = false;    // the client's pause
    bool internal_pause = false;     // ours, armed before a start function
    size_t current_module = 0;
    std::optional<Resume> stepping;  // the step the client asked for at the last stop
    size_t step_depth = 0;
    const Stop* stopped = nullptr;   // while the VM waits in on_stop
    std::optional<Resume> resume;
    std::vector<VarRef> var_refs;    // reset at every stop
};

}

#endif

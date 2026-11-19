// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The DAP server (#54). Framing, endpoint parsing, the virtual WAT and value
// formatting are tested directly; the session tests start the real wasmvm with
// `--dap 0` and talk to it over TCP through the small client below. That client
// is written for this test: it checks the server against this file's reading of
// the protocol, not against a real DAP client.

#include <harness.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include <WasmVM.hpp>
#include <exception.hpp>
#include <json.hpp>

#include "dap/Framing.hpp"
#include "dap/Socket.hpp"
#include "dap/DapServer.hpp"

#ifdef _WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define popen _popen
#  define pclose _pclose
#else
#  include <sys/socket.h>
#  include <sys/time.h>
#  include <sys/wait.h>
#  include <netdb.h>
#  include <unistd.h>
#endif

using namespace Testing;
using Object = Json::Value::Object;
using Array = Json::Value::Array;

// ---- helpers ------------------------------------------------------------

static Json::Value* member(Json::Value& value, const std::string& key){
    if(value.type() != Json::Value::Type::Object){
        return nullptr;
    }
    auto& object = value.get<Object>();
    auto found = object.find(key);
    return found == object.end() ? nullptr : &found->second;
}

// value["a"]["b"]..., nullptr when any step is missing.
static Json::Value* path(Json::Value& value, std::initializer_list<const char*> keys){
    Json::Value* at = &value;
    for(const char* key : keys){
        at = at ? member(*at, key) : nullptr;
    }
    return at;
}

static std::string str(Json::Value* value){
    return (value && value->type() == Json::Value::Type::String) ? value->get<Json::Value::String>() : std::string("<none>");
}

static long long num(Json::Value* value){
    return (value && value->type() == Json::Value::Type::Integer) ? value->get<Json::Value::Integer>() : -999;
}

static bool flag(Json::Value* value){
    return value && value->type() == Json::Value::Type::Bool && value->get<Json::Value::Bool>();
}

static Json::Value& at(Json::Value* array, size_t index){
    static Json::Value missing;
    if(array == nullptr || array->type() != Json::Value::Type::Array || index >= array->get<Array>().size()){
        missing = Json::Value();
        return missing;
    }
    return array->get<Array>()[index];
}

static size_t count(Json::Value* array){
    return (array && array->type() == Json::Value::Type::Array) ? array->get<Array>().size() : 0;
}

static std::string read_file(const std::string& name){
    std::ifstream in(name);
    std::stringstream text;
    text << in.rdbuf();
    return text.str();
}

static WasmVM::WasmModule load(const std::string& wasm){
    std::ifstream in(wasm, std::ios::binary);
    return WasmVM::module_decode(in);
}

// ---- a client for the session tests ------------------------------------

class Session {
public:
    // Starts `wasmvm --no-system --dap 0 <wasm>` and connects to it.
    explicit Session(const std::string& wasm){
        std::string command = std::string("\"" WASMVM_BIN "\" --no-system --dap 0 \"") + wasm + "\" 2>&1";
        process = popen(command.c_str(), "r");
        if(process == nullptr){
            return;
        }
        char line[256] = {0};
        if(std::fgets(line, sizeof(line), process) == nullptr){
            return;
        }
        listening = line;
        size_t colon = listening.rfind(':');
        if(listening.find("DAP server listening on") == std::string::npos || colon == std::string::npos){
            return;
        }
        std::string port = listening.substr(colon + 1);
        while(!port.empty() && (port.back() == '\n' || port.back() == '\r')){
            port.pop_back();
        }
#ifdef _WIN32
        WSADATA data;
        WSAStartup(MAKEWORD(2, 2), &data);
#endif
        addrinfo hints {};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* found = nullptr;
        if(getaddrinfo("127.0.0.1", port.c_str(), &hints, &found) != 0){
            return;
        }
        handle = socket(found->ai_family, found->ai_socktype, found->ai_protocol);
        connected = connect(handle, found->ai_addr, (int)found->ai_addrlen) == 0;
        freeaddrinfo(found);
        // Never hang the suite: a missing message fails after 10 s.
#ifdef _WIN32
        DWORD timeout = 10000;
        setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
        timeval timeout {10, 0};
        setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
    }
    ~Session(){
        close_socket();
        if(process){
            pclose(process);
        }
    }
    bool ok() const { return connected; }

    // Sends a request; `arguments` is JSON text.
    long long send(const std::string& command, const std::string& arguments = "{}"){
        long long id = next_seq++;
        std::string body = "{\"seq\":" + std::to_string(id) + ",\"type\":\"request\",\"command\":\""
            + command + "\",\"arguments\":" + arguments + "}";
        std::string framed = Dap::frame(body);
        ::send(handle, framed.data(), (int)framed.size(), 0);
        return id;
    }
    // The next message; an empty object when the connection closed or timed out.
    Json::Value next(){
        while(true){
            if(std::optional<std::string> body = reader.next()){
                std::istringstream in(body.value());
                Json::Value message;
                in >> message;
                log.push_back(body.value());
                return message;
            }
            char buffer[4096];
            auto got = recv(handle, buffer, sizeof(buffer), 0);
            if(got <= 0){
                return Json::Value();
            }
            reader.feed(buffer, (size_t)got);
        }
    }
    // Skips messages until the response to request `id`.
    Json::Value response(long long id){
        while(true){
            Json::Value message = next();
            if(str(member(message, "type")) == "<none>"){
                return message;
            }
            if(str(member(message, "type")) == "response" && num(member(message, "request_seq")) == id){
                return message;
            }
        }
    }
    Json::Value request(const std::string& command, const std::string& arguments = "{}"){
        return response(send(command, arguments));
    }
    // Skips messages until the event `name`.
    Json::Value event(const std::string& name){
        while(true){
            Json::Value message = next();
            if(str(member(message, "type")) == "<none>"){
                return message;
            }
            if(str(member(message, "event")) == name){
                return message;
            }
        }
    }
    // Ends the session and returns wasmvm's exit status (-1 if unknown).
    int finish(){
        close_socket();
        if(process == nullptr){
            return -1;
        }
        int status = pclose(process);
        process = nullptr;
#ifdef _WIN32
        return status;
#else
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
    }
    // initialize, launch and the breakpoints, then configurationDone.
    void start(const std::string& launch_args, const std::string& breakpoints_on_main){
        request("initialize", "{\"adapterID\":\"test\",\"linesStartAt1\":true}");
        request("launch", launch_args);
        if(!breakpoints_on_main.empty()){
            request("setBreakpoints", "{\"source\":{\"sourceReference\":" + std::to_string(main_reference()) + "},\"breakpoints\":" + breakpoints_on_main + "}");
        }
        request("configurationDone");
    }
    long long reference_of(const std::string& name){
        Json::Value sources = request("loadedSources");
        Json::Value* list = path(sources, {"body", "sources"});
        for(size_t i = 0; i < count(list); ++i){
            if(str(member(at(list, i), "name")) == name){
                return num(member(at(list, i), "sourceReference"));
            }
        }
        return -1;
    }
    long long main_reference(){
        return reference_of("main.wasm:$main");
    }
    // The top frame's line and name.
    std::pair<long long, std::string> top(){
        Json::Value trace = request("stackTrace", "{\"threadId\":1}");
        Json::Value& frame = at(path(trace, {"body", "stackFrames"}), 0);
        return {num(member(frame, "line")), str(member(frame, "name"))};
    }
    // name -> value of one scope ("Locals", "Globals", ...) of frame 1.
    std::map<std::string, std::string> scope(const std::string& name, long long* reference = nullptr){
        std::map<std::string, std::string> result;
        Json::Value scopes = request("scopes", "{\"frameId\":1}");
        Json::Value* list = path(scopes, {"body", "scopes"});
        for(size_t i = 0; i < count(list); ++i){
            if(str(member(at(list, i), "name")) == name){
                long long ref = num(member(at(list, i), "variablesReference"));
                if(reference){
                    *reference = ref;
                }
                Json::Value vars = request("variables", "{\"variablesReference\":" + std::to_string(ref) + "}");
                Json::Value* items = path(vars, {"body", "variables"});
                for(size_t j = 0; j < count(items); ++j){
                    result[str(member(at(items, j), "name"))] = str(member(at(items, j), "value"));
                    long long child = num(member(at(items, j), "variablesReference"));
                    if(child > 0){
                        Json::Value inner = request("variables", "{\"variablesReference\":" + std::to_string(child) + "}");
                        Json::Value* values = path(inner, {"body", "variables"});
                        for(size_t k = 0; k < count(values); ++k){
                            result[str(member(at(items, j), "name")) + str(member(at(values, k), "name"))] = str(member(at(values, k), "value"));
                        }
                    }
                }
            }
        }
        return result;
    }
    std::string listening;
    std::vector<std::string> log;
private:
    void close_socket(){
        if(connected){
#ifdef _WIN32
            closesocket(handle);
#else
            ::close(handle);
#endif
            connected = false;
        }
    }
    FILE* process = nullptr;
#ifdef _WIN32
    SOCKET handle = INVALID_SOCKET;
#else
    int handle = -1;
#endif
    bool connected = false;
    long long next_seq = 1;
    Dap::MessageReader reader;
};

using Vars = std::map<std::string, std::string>;

static const std::string main_wasm = DAP_FIXTURES "/main.wasm";
static const std::string trap_wasm = DAP_FIXTURES "/trap.wasm";

static bool endpoint_throws(const std::string& spec){
    try {
        Dap::Endpoint::parse(spec);
    }catch(WasmVM::Exception::Exception&){
        return true;
    }
    return false;
}

static std::string format(WasmVM::Value value){
    return Dap::Server::format_value(value);
}

Suite dap {
    Category("framing", {
        Test("one_message", {
            Dap::MessageReader reader;
            std::string data = "Content-Length: 2\r\n\r\n{}";
            reader.feed(data.data(), data.size());
            std::optional<std::string> body = reader.next();
            Assert(body.has_value());
            ExpectEq(body.value(), std::string("{}"));
            Expect(!reader.next().has_value());
        })
        Test("split_across_reads", {
            Dap::MessageReader reader;
            std::string data = Dap::frame("{\"a\":1}") + Dap::frame("[]");
            for(char c : data){
                reader.feed(&c, 1);
            }
            ExpectEq(reader.next().value_or("<none>"), std::string("{\"a\":1}"));
            ExpectEq(reader.next().value_or("<none>"), std::string("[]"));
        })
        Test("body_bytes_not_characters", {
            Dap::MessageReader reader;
            std::string body = "\"\xC3\xA9\""; // 4 bytes, 3 characters
            std::string data = Dap::frame(body);
            ExpectEq(data.substr(0, 19), std::string("Content-Length: 4\r\n"));
            reader.feed(data.data(), data.size());
            ExpectEq(reader.next().value_or("<none>"), body);
        })
        Test("other_header_fields_ignored", {
            Dap::MessageReader reader;
            std::string data = "Content-Type: application/json\r\ncontent-length: 2\r\n\r\n[]";
            reader.feed(data.data(), data.size());
            ExpectEq(reader.next().value_or("<none>"), std::string("[]"));
        })
        Test("missing_length_rejected", {
            Dap::MessageReader reader;
            std::string data = "Content-Type: x\r\n\r\n{}";
            reader.feed(data.data(), data.size());
            ExpectThrows(WasmVM::Exception::Exception, reader.next());
        })
        Test("bad_length_rejected", {
            Dap::MessageReader reader;
            std::string data = "Content-Length: 2x\r\n\r\n{}";
            reader.feed(data.data(), data.size());
            ExpectThrows(WasmVM::Exception::Exception, reader.next());
        })
    })
    Category("endpoint", {
        Test("port_only_is_loopback", {
            Dap::Endpoint endpoint = Dap::Endpoint::parse("4711");
            ExpectEq(endpoint.host, std::string("127.0.0.1"));
            ExpectEq(endpoint.port, 4711);
        })
        Test("address_and_port", {
            Dap::Endpoint endpoint = Dap::Endpoint::parse("0.0.0.0:80");
            ExpectEq(endpoint.host, std::string("0.0.0.0"));
            ExpectEq(endpoint.port, 80);
        })
        Test("bracketed_ipv6", {
            Dap::Endpoint endpoint = Dap::Endpoint::parse("[::1]:0");
            ExpectEq(endpoint.host, std::string("::1"));
            ExpectEq(endpoint.port, 0);
        })
        Test("malformed_rejected", {
            Expect(endpoint_throws(""));
            Expect(endpoint_throws("abc"));
            Expect(endpoint_throws("65536"));
            Expect(endpoint_throws("::1:9"));
            Expect(endpoint_throws("[::1]9"));
            Expect(endpoint_throws(":9"));
        })
        Test("listener_reports_actual_port", {
            Dap::Listener listener(Dap::Endpoint::parse("0"));
            std::string address = listener.address();
            Expect(address.rfind("127.0.0.1:", 0) == 0);
            Expect(address != "127.0.0.1:0");
        })
    })
    Category("source", {
        Test("one_instruction_per_line_with_end", {
            WasmVM::WasmModule module = load(main_wasm);
            Dap::Server::Source source = Dap::Server::make_source(module, 1, "main.wasm");
            ExpectEq(source.name, std::string("main.wasm:$main"));
            ExpectEq(source.text, read_file(DAP_SOURCE_DIR "/main.expected"));
            // Line k + 2 is body[k]: as many lines as instructions, plus the header.
            size_t lines = 0;
            for(char c : source.text){
                lines += (c == '\n');
            }
            ExpectEq(lines, module.funcs[1].body.size() + 1);
        })
        Test("end_knows_its_block", {
            WasmVM::WasmModule module = load(main_wasm);
            Dap::Server::Source source = Dap::Server::make_source(module, 1, "main.wasm");
            // body: i32.const, i32.const, call, local.set, block, local.get, global.set, end, end
            Assert(source.opener.size() == 9);
            ExpectEq(source.opener[7], 4);
            ExpectEq(source.opener[8], -1);
        })
        Test("params_named", {
            WasmVM::WasmModule module = load(main_wasm);
            Dap::Server::Source source = Dap::Server::make_source(module, 0, "main.wasm");
            Expect(source.text.rfind("(func $add (type 1) (param $a i32) (param $b i32) (result i32)\n", 0) == 0);
            Expect(source.text.find("  local.get $a\n  local.get $b\n") != std::string::npos);
        })
    })
    Category("value", {
        Test("integers_decimal_and_hex", {
            ExpectEq(format((WasmVM::i32_t)-1), std::string("-1 (0xffffffff)"));
            ExpectEq(format((WasmVM::i64_t)5), std::string("5 (0x0000000000000005)"));
        })
        Test("floats_shortest", {
            ExpectEq(format((WasmVM::f32_t)0.1f), std::string("0.1"));
            ExpectEq(format((WasmVM::f64_t)0.1), std::string("0.1"));
        })
        Test("null_reference", {
            ExpectEq(format(WasmVM::funcref_t()), std::string("null"));
            ExpectEq(format(WasmVM::funcref_t(3)), std::string("func@3"));
        })
    })
    Category("session", {
        Test("breakpoint_step_stack_variables", {
            Session session(main_wasm);
            Assert(session.ok());
            // Line 4 of $main is body[2], `call $add`.
            session.start("{}", "[{\"line\":4}]");
            Json::Value stopped = session.event("stopped");
            ExpectEq(str(path(stopped, {"body", "reason"})), std::string("breakpoint"));
            auto top1 = session.top(); ExpectEq(top1.first, 4LL); ExpectEq(top1.second, std::string("$main"));

            session.request("stepIn", "{\"threadId\":1}");
            stopped = session.event("stopped");
            ExpectEq(str(path(stopped, {"body", "reason"})), std::string("step"));
            Json::Value trace = session.request("stackTrace", "{\"threadId\":1}");
            Json::Value* frames = path(trace, {"body", "stackFrames"});
            AssertEq(count(frames), 2);
            ExpectEq(str(member(at(frames, 0), "name")), std::string("$add"));
            ExpectEq(num(member(at(frames, 0), "line")), 2);
            ExpectEq(str(member(at(frames, 1), "name")), std::string("$main"));
            ExpectEq(num(member(at(frames, 1), "line")), 4);
            ExpectEq(str(path(at(frames, 0), {"source", "name"})), std::string("main.wasm:$add"));

            Vars locals = session.scope("Locals");
            ExpectEq(locals["$a"], std::string("2 (0x00000002)"));
            ExpectEq(locals["$b"], std::string("3 (0x00000003)"));

            session.request("next", "{\"threadId\":1}");
            session.event("stopped");
            auto top2 = session.top(); ExpectEq(top2.first, 3LL); ExpectEq(top2.second, std::string("$add"));
            Vars operands = session.scope("Operand stack");
            ExpectEq(operands["label 0 (func)"], std::string("arity 1, 1 value"));
            ExpectEq(operands["label 0 (func)[0]"], std::string("2 (0x00000002)"));

            session.request("stepOut", "{\"threadId\":1}");
            session.event("stopped");
            // Back in $main at body[3], `local.set $x`.
            auto top3 = session.top(); ExpectEq(top3.first, 5LL); ExpectEq(top3.second, std::string("$main"));

            session.request("next", "{\"threadId\":1}");
            session.event("stopped");
            session.request("next", "{\"threadId\":1}");
            session.event("stopped");
            auto top4 = session.top(); ExpectEq(top4.first, 7LL); ExpectEq(top4.second, std::string("$main"));
            operands = session.scope("Operand stack");
            ExpectEq(operands["label 1 (block)"], std::string("arity 0, 0 values"));
            Vars globals = session.scope("Globals");
            ExpectEq(globals["$counter"], std::string("0 (0x00000000)"));
            ExpectEq(globals["$limit"], std::string("-1 (0xffffffffffffffff)"));

            session.request("continue", "{\"threadId\":1}");
            Json::Value exited = session.event("exited");
            ExpectEq(num(path(exited, {"body", "exitCode"})), 0);
            session.event("terminated");
            session.request("disconnect");
            ExpectEq(session.finish(), 0);
        })
        Test("source_request_serves_virtual_wat", {
            Session session(main_wasm);
            Assert(session.ok());
            session.request("initialize");
            session.request("launch");
            Json::Value reply = session.request("source", "{\"sourceReference\":" + std::to_string(session.main_reference()) + "}");
            ExpectEq(str(path(reply, {"body", "content"})), read_file(DAP_SOURCE_DIR "/main.expected"));
            session.request("disconnect");
            Expect(session.finish() != 0); // terminated before it ran
        })
        Test("stop_on_entry", {
            Session session(main_wasm);
            Assert(session.ok());
            session.start("{\"stopOnEntry\":true}", "");
            Json::Value stopped = session.event("stopped");
            ExpectEq(str(path(stopped, {"body", "reason"})), std::string("entry"));
            auto top5 = session.top(); ExpectEq(top5.first, 2LL); ExpectEq(top5.second, std::string("$main"));
            session.request("continue", "{\"threadId\":1}");
            session.event("terminated");
            session.request("disconnect");
            ExpectEq(session.finish(), 0);
        })
        Test("breakpoint_cleared", {
            Session session(main_wasm);
            Assert(session.ok());
            session.start("{}", "[{\"line\":3},{\"line\":6}]");
            session.event("stopped");
            ExpectEq(session.top().first, 3);
            Json::Value reply = session.request("setBreakpoints", "{\"source\":{\"sourceReference\":" + std::to_string(session.main_reference()) + "},\"breakpoints\":[]}");
            Expect(flag(member(reply, "success")));
            session.request("continue", "{\"threadId\":1}");
            Json::Value next = session.next();
            while(str(member(next, "type")) == "response"){
                next = session.next();
            }
            ExpectEq(str(member(next, "event")), std::string("exited"));
            session.request("disconnect");
            ExpectEq(session.finish(), 0);
        })
        Test("invalid_line_unverified", {
            Session session(main_wasm);
            Assert(session.ok());
            session.request("initialize");
            session.request("launch");
            Json::Value reply = session.request("setBreakpoints", "{\"source\":{\"sourceReference\":" + std::to_string(session.main_reference()) + "},\"breakpoints\":[{\"line\":1},{\"line\":11}]}");
            Json::Value* list = path(reply, {"body", "breakpoints"});
            AssertEq(count(list), 2);
            Expect(!flag(member(at(list, 0), "verified")));
            Expect(!flag(member(at(list, 1), "verified")));
            session.request("disconnect");
            session.finish();
        })
        Test("set_global_and_read_memory", {
            Session session(main_wasm);
            Assert(session.ok());
            session.start("{}", "[{\"line\":9}]"); // the block's `end`, after global.set
            session.event("stopped");
            long long globals = 0;
            ExpectEq(session.scope("Globals", &globals)["$counter"], std::string("5 (0x00000005)"));
            Json::Value reply = session.request("setVariable", "{\"variablesReference\":" + std::to_string(globals) + ",\"name\":\"$counter\",\"value\":\"0x10\"}");
            Expect(flag(member(reply, "success")));
            ExpectEq(session.scope("Globals")["$counter"], std::string("16 (0x00000010)"));
            reply = session.request("setVariable", "{\"variablesReference\":" + std::to_string(globals) + ",\"name\":\"$limit\",\"value\":\"1\"}");
            Expect(!flag(member(reply, "success"))); // immutable

            ExpectEq(session.scope("Memory")["$mem"], std::string("1 pages (65536 bytes)"));
            reply = session.request("readMemory", "{\"memoryReference\":\"mem:0\",\"offset\":65532,\"count\":8}");
            ExpectEq(str(path(reply, {"body", "data"})), std::string("AAAAAA=="));
            ExpectEq(num(path(reply, {"body", "unreadableBytes"})), 4);
            session.request("continue", "{\"threadId\":1}");
            session.event("terminated");
            session.request("disconnect");
            ExpectEq(session.finish(), 0);
        })
        Test("trap_stops_then_exits", {
            Session session(trap_wasm);
            Assert(session.ok());
            session.start("{}", "");
            Json::Value stopped = session.event("stopped");
            ExpectEq(str(path(stopped, {"body", "reason"})), std::string("exception"));
            Json::Value trace = session.request("stackTrace", "{\"threadId\":1}");
            ExpectEq(num(member(at(path(trace, {"body", "stackFrames"}), 0), "line")), 3);
            session.request("continue", "{\"threadId\":1}");
            Json::Value exited = session.event("exited");
            ExpectEq(num(path(exited, {"body", "exitCode"})), -1);
            session.request("disconnect");
            Expect(session.finish() != 0);
        })
        Test("trap_filter_off_does_not_stop", {
            Session session(trap_wasm);
            Assert(session.ok());
            session.request("initialize");
            session.request("launch");
            session.request("setExceptionBreakpoints", "{\"filters\":[]}");
            session.request("configurationDone");
            Json::Value next = session.next();
            while(str(member(next, "type")) == "response"){
                next = session.next();
            }
            ExpectEq(str(member(next, "event")), std::string("exited"));
            session.request("disconnect");
            session.finish();
        })
        Test("disconnect_terminates_while_stopped", {
            Session session(main_wasm);
            Assert(session.ok());
            session.start("{\"stopOnEntry\":true}", "");
            session.event("stopped");
            session.request("disconnect", "{\"terminateDebuggee\":true}");
            ExpectEq(session.finish(), 1);
        })
        Test("disconnect_detaches_and_runs_on", {
            Session session(main_wasm);
            Assert(session.ok());
            session.start("{\"stopOnEntry\":true}", "[{\"line\":6}]");
            session.event("stopped");
            session.request("disconnect", "{\"terminateDebuggee\":false}");
            ExpectEq(session.finish(), 0);
        })
        Test("requests_refused_while_running", {
            Session session(main_wasm);
            Assert(session.ok());
            session.request("initialize");
            session.request("launch");
            // Before configurationDone nothing runs and nothing is stopped.
            Json::Value reply = session.request("stackTrace", "{\"threadId\":1}");
            Expect(!flag(member(reply, "success")));
            reply = session.request("continue", "{\"threadId\":1}");
            Expect(!flag(member(reply, "success")));
            session.request("disconnect");
            session.finish();
        })
    })
};

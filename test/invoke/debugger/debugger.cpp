// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Debugger Layer 1 (#53): breakpoints, next / stepIn / stepOut, pause, and
// stops on traps and on uncaught wasm exceptions, all driven through the
// Debugger interface. Built only with WASMVM_DEBUG (test/invoke/CMakeLists.txt).
//
// The Recorder below answers each stop from a script and writes every stop
// down as "Reason@funcaddr:instr/depth", depth being frames.size() at the
// stop; each test compares the whole trace.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <Debugger.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <functional>
#include <future>
#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

using Reason = Debugger::Reason;
using Resume = Debugger::Resume;

const char* name(Reason reason) {
    switch (reason) {
        case Reason::Breakpoint: return "Breakpoint";
        case Reason::Step:       return "Step";
        case Reason::Pause:      return "Pause";
        case Reason::Trap:       return "Trap";
        case Reason::Exception:  return "Exception";
    }
    return "?";
}

struct Recorder : public Debugger {
    std::string trace;
    std::deque<Resume> script;                 // one answer per stop; Continue once empty
    std::function<void(const Stop&)> on_each;  // optional extra check at each stop
    int stops = 0;
    bool trap_seen = false;
    GcPtr<WasmException> exn_seen;

    Resume on_stop(const Stop& stop) override {
        stops += 1;
        trace += (trace.empty() ? "" : " ") + std::string(name(stop.reason)) + "@"
               + std::to_string(stop.position.funcaddr) + ":"
               + (stop.position.instr == (index_t)-1 ? std::string("<-1>") : std::to_string(stop.position.instr))
               + "/" + std::to_string(stop.stack.frames.size());
        trap_seen = trap_seen || (stop.trap != nullptr);
        if (stop.exn) {
            exn_seen = stop.exn;
        }
        if (on_each) {
            on_each(stop);
        }
        if (script.empty()) {
            return Resume::Continue;
        }
        Resume next = script.front();
        script.pop_front();
        return next;
    }
};

struct Guest {
    Store* store;
    ModuleInst* instance;
    index_t f; // store address of the export "f"
};

// Host functions are added to the store first, so they take store addresses
// 0..hosts-1 and the module's own functions follow.
Guest load(const std::string& wat, std::vector<hostfunc_t> hosts = {}, Debugger* debugger = nullptr) {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store& store = stores.emplace_back();
    ModuleInst& host = instances.emplace_back();
    std::vector<ExternVal> externs;
    for (hostfunc_t body : hosts) {
        externs.push_back(ExternVal{ExternVal::ExternType::Func, (index_t)store.funcs.size()});
        FuncInst& func = store.funcs.emplace_back(host);
        func.body = body;
    }
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, externs, debugger);
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            return Guest{&store, &instance, exported.value.addr};
        }
    }
    return Guest{&store, &instance, (index_t)-1};
}

std::vector<Value> run(const Guest& guest, Debugger* debugger, const std::atomic<bool>* interrupt = nullptr) {
    Stack stack(*guest.store, interrupt, debugger);
    stack.invoke(guest.f, {});
    return stack.run();
}

size_t body_size(const Guest& guest, index_t funcaddr) {
    return std::get<WasmFunc>(guest.store->funcs[funcaddr].body).body.size();
}

// Store address 0: $inner, instructions 0 i32.const 1, 1 i32.const 2, 2 i32.add, 3 end.
// Store address 1: "f",    instructions 0 call $inner, 1 i32.const 10, 2 i32.add, 3 end.
const std::string calls =
    "(module "
    "  (func $inner (result i32) i32.const 1 i32.const 2 i32.add) "
    "  (func (export \"f\") (result i32) call $inner i32.const 10 i32.add))";

int host_calls = 0;
std::vector<Value> counting_host(Stack&) {
    host_calls += 1;
    return {};
}
std::vector<Value> trapping_host(Stack&) {
    throw Exception::Trap("host trap");
}

// Store address 0: the host function; 1: "f",
// instructions 0 call $h, 1 i32.const 5, 2 end.
const std::string calls_host =
    "(module "
    "  (import \"env\" \"h\" (func $h)) "
    "  (func (export \"f\") (result i32) call $h i32.const 5))";

// Store address 0: $inner, instructions 0 i32.const 1, 1 drop, 2 unreachable, 3 end.
// Store address 1: "f", instruction 0 call $inner.
const std::string traps =
    "(module "
    "  (func $inner i32.const 1 drop unreachable) "
    "  (func (export \"f\") call $inner))";

// Store address 0: the trapping host; 1: "f",
// instructions 0 i32.const 0, 1 drop, 2 call $h, 3 end.
const std::string traps_in_host =
    "(module "
    "  (import \"env\" \"h\" (func $h)) "
    "  (func (export \"f\") i32.const 0 drop call $h))";

// Store address 0: $inner, instructions 0 i32.const 7, 1 throw $t, 2 end.
// Store address 1: "f", instruction 0 call $inner.
const std::string throws_uncaught =
    "(module "
    "  (tag $t (param i32)) "
    "  (func $inner i32.const 7 throw $t) "
    "  (func (export \"f\") call $inner))";

// Same $inner; "f" catches the throw one frame up and returns its payload.
const std::string throws_caught =
    "(module "
    "  (tag $t (param i32)) "
    "  (func $inner i32.const 7 throw $t) "
    "  (func (export \"f\") (result i32) "
    "    block $h (result i32) "
    "      try_table (catch $t $h) call $inner end "
    "      i32.const 0 "
    "    end))";

const std::string infinite_loop =
    "(module (func (export \"f\") loop br 0 end))";

// The start function is the module's only function: instructions 0 nop, 1 end.
const std::string has_start =
    "(module (func $s nop) (start $s))";

} // namespace

Suite debugger {
    Category("fixtures", {
        Test("instruction indices in the fixtures are the ones the traces assume", {
            Guest guest = load(calls);
            AssertEq(guest.f, (index_t)1);
            ExpectEq(body_size(guest, 0), (size_t)4);
            ExpectEq(body_size(guest, 1), (size_t)4);
            Guest host = load(calls_host, {&counting_host});
            AssertEq(host.f, (index_t)1);
            ExpectEq(body_size(host, 1), (size_t)3);
            ExpectEq(body_size(load(traps), 0), (size_t)4);
            ExpectEq(body_size(load(traps_in_host, {&trapping_host}), 1), (size_t)4);
            ExpectEq(body_size(load(throws_uncaught), 0), (size_t)3);
        })
    })
    Category("breakpoints", {
        Test("a breakpoint stops before its instruction, frames intact", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.set_breakpoint({0, 1});
            std::vector<Value> r = run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Breakpoint@0:1/2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)13);
        })
        Test("a cleared breakpoint no longer stops", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.set_breakpoint({0, 1});
            dbg.set_breakpoint({1, 1});
            dbg.clear_breakpoint({0, 1});
            run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Breakpoint@1:1/1"));
            Recorder cleared;
            cleared.set_breakpoint({0, 1});
            cleared.set_breakpoint({1, 1});
            cleared.clear_breakpoints();
            run(guest, &cleared);
            ExpectEq(cleared.trace, std::string(""));
        })
    })
    Category("stepping", {
        Test("stepIn enters the call; next walks the callee and returns to the caller", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.pause();
            dbg.script.assign({Resume::StepIn, Resume::StepIn, Resume::Next, Resume::Next, Resume::Next, Resume::Continue});
            std::vector<Value> r = run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Pause@1:0/1 Step@0:0/2 Step@0:1/2 Step@0:2/2 Step@0:3/2 Step@1:1/1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)13);
        })
        Test("next steps over a wasm call", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.pause();
            dbg.script.assign({Resume::Next, Resume::Continue});
            run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Pause@1:0/1 Step@1:1/1"));
        })
        Test("stepOut stops at the caller's next instruction", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.set_breakpoint({0, 0});
            dbg.script.assign({Resume::StepOut, Resume::Continue});
            run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Breakpoint@0:0/2 Step@1:1/1"));
        })
        Test("stepIn on a host call steps over it as one unit", {
            host_calls = 0;
            Guest guest = load(calls_host, {&counting_host});
            Recorder dbg;
            dbg.pause();
            dbg.script.assign({Resume::StepIn, Resume::Continue});
            run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Pause@1:0/1 Step@1:1/1"));
            ExpectEq(host_calls, 1);
        })
    })
    Category("pause", {
        Test("pause stops before the next instruction and does not interrupt", {
            Guest guest = load(calls);
            Recorder dbg;
            dbg.pause();
            std::vector<Value> r = run(guest, &dbg);
            ExpectEq(dbg.trace, std::string("Pause@1:0/1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)13);
        })
        Test("pause from another thread stops a running loop", {
            Guest guest = load(infinite_loop);
            Recorder dbg;
            std::atomic<bool> interrupt(false);
            // Once stopped, end the guest through the interrupt flag.
            dbg.on_each = [&interrupt](const Debugger::Stop&) { interrupt.store(true); };
            std::future<std::string> outcome = std::async(std::launch::async, [&] {
                try {
                    run(guest, &dbg, &interrupt);
                    return std::string("returned");
                } catch (Exception::interrupted&) {
                    return std::string("interrupted");
                } catch (...) {
                    return std::string("other");
                }
            });
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            dbg.pause();
            if (outcome.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
                std::cerr << "guest did not stop within 10s of pause(); aborting suite" << std::endl;
                std::_Exit(1);
            }
            ExpectEq(outcome.get(), std::string("interrupted"));
            ExpectEq(dbg.stops, 1);
            ExpectEq(dbg.trace.rfind("Pause@0:", 0), (size_t)0);
        })
    })
    Category("traps", {
        Test("a wasm trap stops at the trapping instruction, then rethrows", {
            Guest guest = load(traps);
            Recorder dbg;
            dbg.script.assign({Resume::StepIn}); // ignored for a trap
            ExpectThrows(Exception::Trap, run(guest, &dbg));
            ExpectEq(dbg.trace, std::string("Trap@0:2/2"));
            ExpectEq(dbg.trap_seen, true);
        })
        Test("a host trap stops at the call instruction, host frame on top", {
            Guest guest = load(traps_in_host, {&trapping_host});
            Recorder dbg;
            ExpectThrows(Exception::Trap, run(guest, &dbg));
            ExpectEq(dbg.trace, std::string("Trap@1:2/2"));
            ExpectEq(dbg.trap_seen, true);
        })
        Test("a host trap with no wasm caller stops with no instruction position", {
            Guest guest = load(traps_in_host, {&trapping_host});
            Recorder dbg;
            Stack stack(*guest.store, nullptr, &dbg);
            stack.invoke(0, {}); // the host function itself, store address 0
            ExpectThrows(Exception::Trap, stack.run());
            ExpectEq(dbg.trace, std::string("Trap@0:<-1>/1"));
            ExpectEq(dbg.trap_seen, true);
        })
    })
    Category("exceptions", {
        Test("an uncaught throw stops with frames intact, then throws Uncaught", {
            Guest guest = load(throws_uncaught);
            Recorder dbg;
            Stack stack(*guest.store, nullptr, &dbg);
            stack.invoke(guest.f, {});
            ExpectThrows(Exception::Uncaught, stack.run());
            ExpectEq(dbg.trace, std::string("Exception@0:1/2"));
            AssertEq((bool)dbg.exn_seen, true);
            ExpectEq(dbg.exn_seen->tag_addr, guest.instance->tagaddrs[0]);
            AssertEq(dbg.exn_seen->values.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(dbg.exn_seen->values[0]), (i32_t)7);
            // As without a debugger, run() leaves the frames unwound.
            ExpectEq(stack.frames.size(), (size_t)0);
        })
        Test("a throw caught one frame up does not stop", {
            Guest guest = load(throws_caught);
            Recorder dbg;
            std::vector<Value> r = run(guest, &dbg);
            ExpectEq(dbg.trace, std::string(""));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)7);
        })
    })
    Category("attach points", {
        Test("module_instanciate runs the start function under the debugger", {
            Recorder dbg;
            dbg.pause();
            Guest guest = load(has_start, {}, &dbg);
            ExpectEq(dbg.trace, std::string("Pause@" + std::to_string(guest.instance->funcaddrs[0]) + ":0/1"));
        })
        Test("a null debugger runs as before", {
            Guest guest = load(calls);
            std::vector<Value> r = run(guest, nullptr);
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)13);
            AssertEq(run(load(throws_caught), nullptr).size(), (size_t)1);
            ExpectThrows(Exception::Uncaught, run(load(throws_uncaught), nullptr));
        })
    })
};

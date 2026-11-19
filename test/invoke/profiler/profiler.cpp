// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Profiler (#61): instruction and call counts, function entry and exit across
// return, tail calls, exception unwinding, traps and interrupts, one section
// per Stack::run(), names, the folded output and, with WASMVM_DEBUG too,
// debugger stop time left out. Built only with WASMVM_PROFILE
// (test/invoke/CMakeLists.txt).
//
// Time comes from a fake clock that only the host function `tick` moves,
// by 10 ns a call, so every time in a report is exact.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <Profiler.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>
#ifdef WASMVM_DEBUG
#include <Debugger.hpp>
#endif

#include <atomic>
#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

int64_t fake_time = 0;
int64_t fake_clock() {
    return fake_time;
}

std::vector<Value> tick(Stack&) {
    fake_time += 10;
    return {};
}

std::atomic<bool> interrupt_flag {false};
std::vector<Value> tick_and_interrupt(Stack&) {
    fake_time += 10;
    interrupt_flag.store(true);
    return {};
}

struct Guest {
    Store* store;
    ModuleInst* host;
    ModuleInst* instance;
    index_t f; // store address of the export "f"
};

Observers with(Profiler* profiler) {
    Observers observers;
    observers.profiler = profiler;
    return observers;
}

// Every module imports env.tick at function index 0. The host module exports
// it as "tick" at store address 0; `second` adds a second host function.
Guest load(const std::string& wat, Profiler* profiler = nullptr, hostfunc_t second = nullptr) {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    fake_time = 0;
    interrupt_flag.store(false);
    Store& store = stores.emplace_back();
    ModuleInst& host = instances.emplace_back();
    std::vector<ExternVal> externs;
    std::vector<std::pair<std::string, hostfunc_t>> hosts {{"tick", &tick}};
    if (second != nullptr) {
        hosts.emplace_back("second", second);
    }
    for (auto& [name, body] : hosts) {
        index_t addr = (index_t)store.funcs.size();
        externs.push_back(ExternVal{ExternVal::ExternType::Func, addr});
        FuncInst& func = store.funcs.emplace_back(host);
        func.body = body;
        host.funcaddrs.push_back(addr);
        host.exports.push_back(ExportInst{name, ExternVal{ExternVal::ExternType::Func, addr}});
    }
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    ModuleInst& instance = instances.emplace_back();
    if (profiler != nullptr) {
        profiler->set_module_name(host, "env");
        profiler->set_module_name(instance, "m");
    }
    module_instanciate(instance, store, module, externs, with(profiler));
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            return Guest{&store, &host, &instance, exported.value.addr};
        }
    }
    return Guest{&store, &host, &instance, (index_t)-1};
}

std::vector<Value> run(const Guest& guest, Profiler& profiler, const std::atomic<bool>* interrupt = nullptr) {
    Stack stack(*guest.store, interrupt, with(&profiler));
    stack.invoke(guest.f, {});
    return stack.run();
}

Json::Value::Array sections(Profiler& profiler) {
    Json::Value report = profiler.report();
    return report.get<Json::Value::Object>()["stacks"].get<Json::Value::Array>();
}

// The function named `name` ("$inner", "tick") in section `index`; an empty
// object when it is not listed.
Json::Value::Object function(Profiler& profiler, size_t index, const std::string& name) {
    Json::Value::Array stacks = sections(profiler);
    if (index >= stacks.size()) {
        return {};
    }
    for (Json::Value& entry : stacks[index].get<Json::Value::Object>()["functions"].get<Json::Value::Array>()) {
        Json::Value::Object& object = entry.get<Json::Value::Object>();
        if (object["name"].get<std::string>() == name) {
            return object;
        }
    }
    return {};
}

int64_t field(Json::Value::Object object, const std::string& key) {
    if (!object.contains(key)) {
        return -1;
    }
    return object[key].get<Json::Value::Integer>();
}

int64_t instr(Json::Value::Object object, const std::string& mnemonic) {
    if (!object.contains("instructions")) {
        return -1;
    }
    Json::Value::Object& counts = object["instructions"].get<Json::Value::Object>();
    return counts.contains(mnemonic) ? counts[mnemonic].get<Json::Value::Integer>() : 0;
}

// The call count and inclusive / exclusive time of `name`, "calls/incl/excl".
std::string timing(Profiler& profiler, size_t index, const std::string& name) {
    Json::Value::Object object = function(profiler, index, name);
    return std::to_string(field(object, "calls")) + "/" + std::to_string(field(object, "inclusive_ns"))
         + "/" + std::to_string(field(object, "exclusive_ns"));
}

const std::string counts =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $inner i32.const 1 drop) "
    "  (func (export \"f\") call $inner call $inner))";

// $a returns from inside a block; f then ticks once more.
const std::string returns =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $a block call $tick return end call $tick) "
    "  (func (export \"f\") call $a call $tick))";

// $a tail-calls $b, which ticks; f ticks afterwards.
const std::string tail_call =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $b call $tick) "
    "  (func $a call $tick return_call $b) "
    "  (func (export \"f\") call $a call $tick))";
const std::string tail_call_indirect =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (type $v (func)) "
    "  (table 1 funcref) "
    "  (func $b call $tick) "
    "  (elem (i32.const 0) $b) "
    "  (func $a call $tick i32.const 0 return_call_indirect (type $v)) "
    "  (func (export \"f\") call $a call $tick))";
const std::string tail_call_ref =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (type $v (func)) "
    "  (func $b (type $v) call $tick) "
    "  (elem declare func $b) "
    "  (func $a call $tick ref.func $b return_call_ref $v) "
    "  (func (export \"f\") call $a call $tick))";

// $b ticks and throws; f catches it two frames up, then ticks.
const std::string throws_caught =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (tag $t) "
    "  (func $b call $tick throw $t) "
    "  (func $a call $b call $tick) "
    "  (func (export \"f\") "
    "    block $h try_table (catch_all $h) call $a end end "
    "    call $tick))";
const std::string throws_uncaught =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (tag $t) "
    "  (func $a call $tick throw $t) "
    "  (func (export \"f\") call $a call $tick))";

const std::string traps =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $a call $tick unreachable) "
    "  (func (export \"f\") call $a call $tick))";

// The second host function sets the interrupt flag; the loop never ends.
const std::string interrupts =
    "(module (import \"env\" \"tick\" (func $tick)) (import \"env\" \"second\" (func $stop)) "
    "  (func $a call $stop loop br 0 end) "
    "  (func (export \"f\") call $a))";

// $r(n) ticks and recurses until n is 0: three calls, two ticks.
const std::string recursion =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $r (param i32) "
    "    local.get 0 if call $tick local.get 0 i32.const 1 i32.sub call $r end) "
    "  (func (export \"f\") i32.const 2 call $r))";

// A start function, then f run on a Stack of its own: two sections.
const std::string with_start =
    "(module (import \"env\" \"tick\" (func $tick)) "
    "  (func $s call $tick) (start $s) "
    "  (func (export \"f\") call $tick call $tick))";

} // namespace

Suite profiler {
    Category("counts", {
        Test("instructions per function and opcode, and calls", {
            Profiler profiler(&fake_clock);
            Guest guest = load(counts);
            run(guest, profiler);
            Json::Value::Object inner = function(profiler, 0, "$inner");
            ExpectEq(field(inner, "calls"), (int64_t)2);
            ExpectEq(instr(inner, "i32.const"), (int64_t)2);
            ExpectEq(instr(inner, "drop"), (int64_t)2);
            ExpectEq(instr(inner, "end"), (int64_t)2);
            ExpectEq(field(inner, "instructions_total"), (int64_t)6);
            Json::Value::Object f = function(profiler, 0, "f");
            ExpectEq(field(f, "calls"), (int64_t)1);
            ExpectEq(instr(f, "call"), (int64_t)2);
            ExpectEq(instr(f, "end"), (int64_t)1);
            ExpectEq(field(f, "instructions_total"), (int64_t)3);
        })
        Test("a host function has calls and time but no instruction counts", {
            Profiler profiler(&fake_clock);
            Guest guest = load(returns, &profiler);
            run(guest, profiler);
            Json::Value::Object host = function(profiler, 0, "tick");
            ExpectEq(host["host"].get<bool>(), true);
            ExpectEq(host["module"].get<std::string>(), std::string("env"));
            ExpectEq(field(host, "calls"), (int64_t)2);
            ExpectEq(host.contains("instructions"), false);
            ExpectEq(host.contains("instructions_total"), false);
        })
    })
    Category("enter and exit", {
        Test("return exits the function at the return", {
            Profiler profiler(&fake_clock);
            Guest guest = load(returns);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/20/0"));
            ExpectEq(timing(profiler, 0, "tick"), std::string("2/20/20"));
        })
        Test("return_call exits the caller and enters the callee under the original caller", {
            Profiler profiler(&fake_clock);
            Guest guest = load(tail_call, &profiler);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "$b"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/30/0"));
            ExpectEq(profiler.folded(),
                std::string("m:f 0\nm:f;env:tick 10\nm:f;m:$b 0\nm:f;m:$b;env:tick 10\nm:f;m:$a 0\nm:f;m:$a;env:tick 10\n"));
        })
        Test("return_call_indirect exits the caller", {
            Profiler profiler(&fake_clock);
            Guest guest = load(tail_call_indirect);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "$b"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/30/0"));
        })
        Test("return_call_ref exits the caller", {
            Profiler profiler(&fake_clock);
            Guest guest = load(tail_call_ref);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "$b"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/30/0"));
        })
        Test("a caught throw exits every unwound frame at the throw", {
            Profiler profiler(&fake_clock);
            Guest guest = load(throws_caught);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$b"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/20/0"));
            ExpectEq(timing(profiler, 0, "tick"), std::string("2/20/20"));
        })
        Test("an uncaught throw exits every frame", {
            Profiler profiler(&fake_clock);
            Guest guest = load(throws_uncaught);
            ExpectThrows(Exception::Uncaught, run(guest, profiler));
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/10/0"));
        })
        Test("a trap exits every frame", {
            Profiler profiler(&fake_clock);
            Guest guest = load(traps);
            ExpectThrows(Exception::Trap, run(guest, profiler));
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/10/0"));
        })
        Test("an interrupt exits every frame", {
            Profiler profiler(&fake_clock);
            Guest guest = load(interrupts, nullptr, &tick_and_interrupt);
            ExpectThrows(Exception::interrupted, run(guest, profiler, &interrupt_flag));
            ExpectEq(timing(profiler, 0, "second"), std::string("1/10/10"));
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/10/0"));
        })
        Test("recursion counts every call but inclusive time only once", {
            Profiler profiler(&fake_clock);
            Guest guest = load(recursion);
            run(guest, profiler);
            ExpectEq(timing(profiler, 0, "$r"), std::string("3/20/0"));
        })
    })
    Category("sections", {
        Test("each Stack::run() is a section of its own", {
            Profiler profiler(&fake_clock);
            Guest guest = load(with_start, &profiler);
            run(guest, profiler);
            Json::Value::Array stacks = sections(profiler);
            AssertEq(stacks.size(), (size_t)2);
            ExpectEq(stacks[0].get<Json::Value::Object>()["entry"].get<std::string>(), std::string("m:$s"));
            ExpectEq(stacks[1].get<Json::Value::Object>()["entry"].get<std::string>(), std::string("m:f"));
            ExpectEq(timing(profiler, 0, "tick"), std::string("1/10/10"));
            ExpectEq(timing(profiler, 1, "tick"), std::string("2/20/20"));
            ExpectEq(field(function(profiler, 0, "f"), "calls"), (int64_t)-1);
            ExpectEq(field(function(profiler, 1, "$s"), "calls"), (int64_t)-1);
        })
    })
    Category("names", {
        Test("frames are module:name, with ';' and spaces replaced", {
            Profiler profiler(&fake_clock);
            Guest guest = load(counts);
            profiler.set_module_name(*guest.instance, "my mod;1");
            run(guest, profiler);
            ExpectEq(profiler.folded(), std::string("my_mod_1:f 0\nmy_mod_1:f;my_mod_1:$inner 0\n"));
            Json::Value::Array stacks = sections(profiler);
            AssertEq(stacks.size(), (size_t)1);
            ExpectEq(stacks[0].get<Json::Value::Object>()["entry"].get<std::string>(), std::string("my mod;1:f"));
        })
        Test("without a module name a frame is the function name alone", {
            Profiler profiler(&fake_clock);
            Guest guest = load(counts);
            run(guest, profiler);
            Json::Value::Object f = function(profiler, 0, "f");
            ExpectEq(f["module"].get<std::string>(), std::string(""));
            ExpectEq(profiler.folded(), std::string("f 0\nf;$inner 0\n"));
        })
    })
#ifdef WASMVM_DEBUG
    Category("debugger", {
        Test("time stopped at a breakpoint is not counted", {
            struct Stopper : public Debugger {
                int stops = 0;
                Resume on_stop(const Stop&) override {
                    stops += 1;
                    fake_time += 1000;
                    return Resume::Continue;
                }
            } dbg;
            Profiler profiler(&fake_clock);
            Guest guest = load(returns);
            // $a is store address 1: break on its first instruction.
            dbg.set_breakpoint(Debugger::Position {1, 0});
            Observers observers = with(&profiler);
            observers.debugger = &dbg;
            Stack stack(*guest.store, nullptr, observers);
            stack.invoke(guest.f, {});
            stack.run();
            AssertEq(dbg.stops, 1);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
            ExpectEq(timing(profiler, 0, "f"), std::string("1/20/0"));
        })
        Test("time stopped at a trap is not counted", {
            struct Stopper : public Debugger {
                int stops = 0;
                Resume on_stop(const Stop&) override {
                    stops += 1;
                    fake_time += 1000;
                    return Resume::Continue;
                }
            } dbg;
            Profiler profiler(&fake_clock);
            Guest guest = load(traps);
            Observers observers = with(&profiler);
            observers.debugger = &dbg;
            Stack stack(*guest.store, nullptr, observers);
            stack.invoke(guest.f, {});
            ExpectThrows(Exception::Trap, stack.run());
            AssertEq(dbg.stops, 1);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
        })
        Test("time stopped at an uncaught exception is not counted", {
            struct Stopper : public Debugger {
                int stops = 0;
                Resume on_stop(const Stop&) override {
                    stops += 1;
                    fake_time += 1000;
                    return Resume::Continue;
                }
            } dbg;
            Profiler profiler(&fake_clock);
            Guest guest = load(throws_uncaught);
            Observers observers = with(&profiler);
            observers.debugger = &dbg;
            Stack stack(*guest.store, nullptr, observers);
            stack.invoke(guest.f, {});
            ExpectThrows(Exception::Uncaught, stack.run());
            AssertEq(dbg.stops, 1);
            ExpectEq(timing(profiler, 0, "$a"), std::string("1/10/0"));
        })
    })
#endif
};

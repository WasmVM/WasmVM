// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// External interruption of Stack::run().
//
// The embedder owns a std::atomic<bool> and hands a pointer to the Stack
// constructor; setting it (typically from another thread) makes run() throw
// Exception::interrupted at the top of the interpreter loop. Two semantics
// these tests pin down:
//
//  1. The interrupt must NOT be catchable by the guest. run()'s only handler
//     is catch(WasmThrow&), so Exception::interrupted propagates straight
//     through try_table/catch_all. The "catch_all cannot swallow" test below
//     turns red if anyone rewires the interrupt into a wasm-level exception.
//  2. An interrupted Stack is abandoned — there is no resume, so nothing here
//     touches a Stack after its run() threw.
//
// Hang insurance: the cross-thread tests wait on a future with a timeout and
// _Exit on expiry (a stuck interpreter thread cannot be joined); the ctest
// TIMEOUT property on this suite is the outer backstop.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>
#include <invoke/exception.hpp>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <future>
#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

// Parse a module from WAT source, validate it, instantiate it, and call its
// single exported function "f" on a Stack carrying the interrupt flag.
std::vector<Value> run(const std::string& wat, const std::atomic<bool>* interrupt) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }

    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store&      store    = stores.emplace_back();
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, {});

    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store, interrupt);
            stack.invoke(exported.value.addr, {});
            return stack.run();
        }
    }
    throw Exception::Exception("test module has no exported function 'f'");
}

const std::string infinite_loop =
    "(module (func (export \"f\") loop br 0 end))";

// The guest wraps its infinite loop in try_table/catch_all: a wasm-level
// exception would be swallowed and "f" would return normally.
const std::string catch_all_loop =
    "(module (func (export \"f\") try_table (catch_all 0) loop br 0 end end))";

enum RunOutcome { outcome_interrupted, outcome_returned, outcome_other };

// Run the guest on a worker thread, set the flag from this thread, and report
// how the guest ended. _Exits the suite if the guest never stops: a stuck
// interpreter thread cannot be joined, and a hang must read as red, not block
// the whole test run.
RunOutcome interrupt_from_here(const std::string& wat) {
    std::atomic<bool> flag(false);
    std::future<RunOutcome> outcome = std::async(std::launch::async, [&wat, &flag] {
        try {
            run(wat, &flag);
            return outcome_returned;
        } catch (Exception::interrupted&) {
            return outcome_interrupted;
        } catch (...) {
            return outcome_other;
        }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    flag.store(true);
    if (outcome.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
        // stderr, unbuffered by std::endl: the harness captures per-test stdout,
        // and _Exit skips all cleanup, so a cout message would be lost with it.
        std::cerr << "guest did not stop within 10s of the interrupt; aborting suite" << std::endl;
        std::_Exit(1);
    }
    return outcome.get();
}

} // namespace

Suite interrupt {
    Category("interruption", {
        Test("a pre-set flag interrupts an infinite loop immediately", {
            std::atomic<bool> flag(true);
            ExpectThrows(Exception::interrupted, run(infinite_loop, &flag));
        })
        Test("a flag set from another thread interrupts a running loop", {
            ExpectEq((int)interrupt_from_here(infinite_loop), (int)outcome_interrupted);
        })
        Test("guest try_table/catch_all cannot swallow the interrupt", {
            ExpectEq((int)interrupt_from_here(catch_all_loop), (int)outcome_interrupted);
        })
    })
    Category("non-interference", {
        Test("an unset flag leaves normal execution untouched", {
            std::atomic<bool> flag(false);
            std::vector<Value> r = run(
                "(module (func (export \"f\") (result i32) i32.const 42))", &flag);
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)42);
        })
    })
};

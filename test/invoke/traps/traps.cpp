// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The exception contract of Stack::run() as seen by an embedder.
//
// This suite deliberately includes only headers WasmVM installs (everything
// under src/include). In particular it never includes the private
// <invoke/exception.hpp>: an embedder consuming an installed WasmVM cannot
// name the concrete trap types, so everything below is written against the
// three public types a `catch` is allowed to see:
//
//   Exception::Trap        the guest hit a spec-defined trap
//   Exception::Uncaught    a wasm `throw` no guest try_table caught
//   Exception::interrupted the embedder's interrupt flag was set
//
// If a private leaf ever stops deriving from Trap, or the throw sites are
// rewired to a bare Exception, the corresponding test here goes red.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <atomic>
#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

// A parsed, validated and instantiated module whose exported "f" is called
// on demand. Kept as an object so a test can look at the instance (e.g. its
// tag addresses) after the call threw.
struct Guest {
    explicit Guest(const std::string& wat, const std::atomic<bool>* interrupt = nullptr)
        : interrupt(interrupt) {
        std::istringstream source(wat);
        WasmModule module = module_parse(source);
        if (std::optional<Exception::Exception> error = module_validate(module)) {
            throw *error;
        }
        static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
        static std::list<Store>      stores;
        store    = &stores.emplace_back();
        instance = &instances.emplace_back();
        module_instanciate(*instance, *store, module, {});
    }

    std::vector<Value> call() {
        for (ExportInst& exported : instance->exports) {
            if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
                Stack stack(*store, interrupt);
                stack.invoke(exported.value.addr, {});
                return stack.run();
            }
        }
        throw Exception::Exception("test module has no exported function 'f'");
    }

    Store*                   store;
    ModuleInst*              instance;
    const std::atomic<bool>* interrupt;
};

std::vector<Value> run(const std::string& wat, const std::atomic<bool>* interrupt = nullptr) {
    return Guest(wat, interrupt).call();
}

// Which public type a run() escaped through. `other` covers both a bare
// Exception and anything not derived from Exception at all.
enum Escape { escape_none, escape_trap, escape_uncaught, escape_interrupted, escape_other };

Escape escape_of(const std::string& wat, const std::atomic<bool>* interrupt = nullptr) {
    try {
        run(wat, interrupt);
        return escape_none;
    } catch (Exception::Trap&) {
        return escape_trap;
    } catch (Exception::Uncaught&) {
        return escape_uncaught;
    } catch (Exception::interrupted&) {
        return escape_interrupted;
    } catch (...) {
        return escape_other;
    }
}

// call_indirect fixtures. Type 0 is [] -> [i32], type 1 is [] -> []; func 0
// has type 1, and "f" calls through table slot 0 expecting type 0.
const std::string call_indirect_prefix =
    "(module (type (func (result i32))) (type (func)) (func) ";
const std::string call_indirect_body =
    "(func (export \"f\") (result i32) i32.const 0 call_indirect (type 0))";

} // namespace

Suite traps {
    Category("guest traps are Exception::Trap", {
        Test("unreachable", {
            ExpectEq((int)escape_of("(module (func (export \"f\") unreachable))"), (int)escape_trap);
        })
        Test("integer divide by zero", {
            ExpectEq((int)escape_of(
                "(module (func (export \"f\") (result i32) i32.const 1 i32.const 0 i32.div_s))"),
                (int)escape_trap);
        })
        Test("integer overflow", {
            ExpectEq((int)escape_of(
                "(module (func (export \"f\") (result i32) i32.const -2147483648 i32.const -1 i32.div_s))"),
                (int)escape_trap);
        })
        Test("invalid conversion to integer", {
            ExpectEq((int)escape_of(
                "(module (func (export \"f\") (result i32) f32.const 3e38 i32.trunc_f32_s))"),
                (int)escape_trap);
        })
        Test("out-of-bounds memory access", {
            ExpectEq((int)escape_of(
                "(module (memory 1) (func (export \"f\") (result i32) i32.const 65536 i32.load))"),
                (int)escape_trap);
        })
        Test("out-of-bounds table access", {
            ExpectEq((int)escape_of(
                "(module (table 1 funcref) (func (export \"f\") "
                "i32.const 0 ref.null func i32.const 2 table.fill 0))"),
                (int)escape_trap);
        })
        Test("call_indirect: index outside the table (undefined element)", {
            ExpectEq((int)escape_of(
                call_indirect_prefix + "(table 1 funcref) "
                "(func (export \"f\") (result i32) i32.const 5 call_indirect (type 0)))"),
                (int)escape_trap);
        })
        Test("call_indirect: null table slot (uninitialized element)", {
            ExpectEq((int)escape_of(
                call_indirect_prefix + "(table 1 funcref) " + call_indirect_body + ")"),
                (int)escape_trap);
        })
        Test("call_indirect: type mismatch", {
            ExpectEq((int)escape_of(
                call_indirect_prefix + "(table funcref (elem 0)) " + call_indirect_body + ")"),
                (int)escape_trap);
        })
        Test("null reference", {
            ExpectEq((int)escape_of(
                "(module (func (export \"f\") (result funcref) ref.null func ref.as_non_null))"),
                (int)escape_trap);
        })
        Test("a Trap is still an Exception, with its message intact", {
            try {
                run("(module (func (export \"f\") unreachable))");
                Expect(false);
            } catch (Exception::Exception& e) {
                ExpectEq(std::string(e.what()), std::string("unreachable"));
            }
        })
    })
    Category("uncaught wasm throw is Exception::Uncaught", {
        Test("escapes as Uncaught, not as a Trap", {
            ExpectEq((int)escape_of(
                "(module (tag $t) (func (export \"f\") throw $t))"),
                (int)escape_uncaught);
        })
        Test("carries the tag address and payload", {
            Guest guest("(module (tag $t (param i32)) (func (export \"f\") i32.const 7 throw $t))");
            try {
                guest.call();
                Expect(false);
            } catch (Exception::Uncaught& e) {
                Assert(e.exn != nullptr);
                AssertEq(guest.instance->tagaddrs.size(), (size_t)1);
                ExpectEq(e.exn->tag_addr, guest.instance->tagaddrs[0]);
                AssertEq(e.exn->values.size(), (size_t)1);
                ExpectEq(std::get<i32_t>(e.exn->values[0]), (i32_t)7);
                ExpectEq(std::string(e.what()), std::string("unhandled WebAssembly exception"));
            }
        })
        Test("a throw the guest catches never reaches the embedder", {
            ExpectEq((int)escape_of(
                "(module (tag $t) (func (export \"f\") (result i32) "
                "block $h try_table (catch_all $h) throw $t end unreachable end i32.const 1))"),
                (int)escape_none);
        })
    })
    Category("interrupt is Exception::interrupted", {
        Test("escapes as interrupted, not as a Trap", {
            std::atomic<bool> flag(true);
            ExpectEq((int)escape_of("(module (func (export \"f\") loop br 0 end))", &flag),
                     (int)escape_interrupted);
        })
    })
};

// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// What a branch leaves on the stack.
//
// A branch to a label carrying results reads the operands off that label and
// puts the top `arity` of them back. When the target is the label the operands
// were read from -- every branch to depth 0, and the fall-through at `else` --
// the read used to copy rather than take, so the operands were put back on top
// of themselves. Nothing downstream trimmed them: `end` forwards whatever the
// label holds, so a function declaring one result handed its embedder two
// values (issue #16).
//
// The counts below are the point of this suite. A `wasmvm` run would not have
// shown any of it: the CLI takes the top of the stack and the top was right.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

// Parse a module from WAT source, validate it, instantiate it, and call its
// single exported function "f".
std::vector<Value> run(const std::string& wat) {
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
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            return stack.run();
        }
    }
    throw Exception::Exception("test module has no exported function 'f'");
}

// A function returning one i32, whose body is `body`.
std::string func_i32(const std::string& body) {
    return "(module (func (export \"f\") (result i32) " + body + "))";
}

} // namespace

Suite branch {
    Category("a branch to depth 0 hands on its operands once", {
        // The issue's reproduction. `block` ends with [5], the add makes [6],
        // and that is the whole of what the function returns.
        Test("br", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) i32.const 5 br 0 end i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
        Test("br_if taken", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) i32.const 5 i32.const 1 br_if 0 end"
                " i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
        Test("br_table", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) i32.const 5 i32.const 0 br_table 0 end"
                " i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
        Test("br_on_null", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) i32.const 5 ref.null func br_on_null 0 drop end"
                " i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
        // `else` is reached by falling out of the then-branch, and it hands the
        // then-branch's results to the same label it read them from.
        Test("falling through to else", {
            std::vector<Value> results = run(func_i32(
                "i32.const 1 if (result i32) i32.const 5 else i32.const 9 end"
                " i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
    })
    Category("branching keeps the right values, not just the right number", {
        // Deeper targets always worked: the label the operands came off is
        // popped on the way out, and its copy goes with it. Kept as the
        // control -- if this one ever breaks, the fault is not in the taking.
        Test("br to depth 1", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) block (result i32) i32.const 5 br 1 end end"
                " i32.const 1 i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 6);
        })
        // A branch empties the label it targets, so it must not reach past it:
        // 100 belongs to the enclosing label and has to still be there.
        Test("a value outside the block survives a br 0", {
            std::vector<Value> results = run(func_i32(
                "i32.const 100 block (result i32) i32.const 5 br 0 end i32.add"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 105);
        })
        // `br` is stack-polymorphic: only the top `arity` operands go to the
        // label, and 7 is dropped rather than returned alongside 5.
        Test("operands under the branch operand are dropped", {
            std::vector<Value> results = run(func_i32(
                "block (result i32) i32.const 7 i32.const 5 br 0 end"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 5);
        })
        // A label with no results keeps nothing at all.
        Test("br 0 to a block with no result returns nothing", {
            std::vector<Value> results = run(
                "(module (func (export \"f\") block i32.const 7 br 0 end))");
            ExpectEq(results.size(), 0U);
        })
        Test("return from inside a block returns one value", {
            std::vector<Value> results = run(func_i32(
                "block i32.const 42 return end i32.const 0"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 42);
        })
        // A loop's label is also the branch target at depth 0, and its back
        // edge runs many times: whatever a branch does to the label has to
        // leave the next iteration a clean stack.
        Test("a counting loop still terminates with one value", {
            std::vector<Value> results = run(func_i32(
                "(local $i i32) loop $l local.get $i i32.const 1 i32.add"
                " local.set $i local.get $i i32.const 3 i32.lt_s br_if $l end"
                " local.get $i"));
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 3);
        })
    })
};

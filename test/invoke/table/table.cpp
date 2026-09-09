// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// What table.grow answers.
//
// The spec has it return the table's length *before* the growth, or -1 if it
// could not grow. It returned the new length, so a guest using the result to
// find where its new slots begin was handed the end instead of the start, and
// on a table that had just doubled it was off by the whole original length.
// memory.grow next door gets this right and says so in a comment; nothing
// covered table.grow's result at all.

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

i32_t i32_at(const std::vector<Value>& values, size_t index) {
    return std::get<i32_t>(values.at(index));
}

} // namespace

Suite invoke_table {
    Category("table.grow", {
        Test("it answers with the length before the growth", {
            std::vector<Value> results = run(
                "(module (table 1 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const 2 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1); // what it was
            ExpectEq(i32_at(results, 1), 3); // what it is
        })
        Test("growing by nothing still answers with the length", {
            std::vector<Value> results = run(
                "(module (table 3 funcref) (func (export \"f\") (result i32)"
                " ref.null func i32.const 0 table.grow 0))");
            AssertEq(results.size(), 1U);
            ExpectEq(i32_at(results, 0), 3);
        })
        Test("the answer is the start of the new slots", {
            // The whole use of the result: index it and reach the first slot
            // that was just added.
            std::vector<Value> results = run(
                "(module (table 1 externref) (func (export \"f\") (result i32)"
                " (local $at i32)"
                " ref.null extern i32.const 2 table.grow 0 local.set $at"
                " local.get $at table.get 0 ref.is_null))");
            AssertEq(results.size(), 1U);
            ExpectEq(i32_at(results, 0), 1);
        })
        Test("growing past the maximum answers -1 and changes nothing", {
            std::vector<Value> results = run(
                "(module (table 1 2 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const 5 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("the given reference fills the new slots", {
            std::vector<Value> results = run(
                "(module (table 1 externref) (func (export \"f\") (result i32 i32)"
                " ref.null extern i32.const 2 table.grow 0 drop"
                " i32.const 1 table.get 0 ref.is_null"
                " i32.const 2 table.get 0 ref.is_null))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("it grows a table of concrete references too", {
            std::vector<Value> results = run(
                "(module (type $s (sub (struct (field i32)))) (table 1 (ref null $s))"
                " (func (export \"f\") (result i32 i32)"
                " ref.null $s i32.const 3 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 4);
        })
        // memory.grow is the instruction table.grow was out of step with.
        Test("memory.grow answers the same way", {
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 2 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 1);
            ExpectEq(i32_at(results, 1), 3);
        })
    })
};

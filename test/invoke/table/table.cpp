// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// What table.grow answers, and where an access is allowed to reach.
//
// The spec has it return the table's length *before* the growth, or -1 if it
// could not grow. It returned the new length, so a guest using the result to
// find where its new slots begin was handed the end instead of the start, and
// on a table that had just doubled it was off by the whole original length.
// memory.grow next door gets this right and says so in a comment; nothing
// covered table.grow's result at all.
//
// The second half is the bounds checks, which every access in memory.cpp and
// table.cpp shared and every one of which could wrap. An address or a count
// arrived as an i32 and was widened with its sign, so -1 became the top of
// the 64-bit address space rather than 4294967295; the checks then read
// `(start + count) > size`, which comes back in range once the addition
// wraps. A load at -1 passed the check and read outside the memory, a store
// at -1 with offset 1 wrapped to zero and wrote at the start of it, and grow
// with -1 shrank instead of refusing.

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

// Whether running the module's "f" traps, rather than answering or escaping
// as something else.
bool traps(const std::string& wat) {
    try {
        run(wat);
    } catch (Exception::Trap&) {
        return true;
    } catch (Exception::Exception& e) {
        std::cout << "      not a trap: " << e.what() << std::endl;
        return false;
    }
    std::cout << "      did not trap" << std::endl;
    return false;
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
    // An address or a count arriving as -1 means 4294967295, which is out of
    // range for any of these -- not a step backwards, and never a way to
    // reach outside.
    Category("an access near the top of the address space", {
        Test("a load at -1 traps instead of reading outside", {
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -1 i32.load))"));
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -1 i32.load offset=1))"));
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const -4 i32.load offset=4))"));
        })
        Test("a store at -1 traps instead of writing at zero", {
            // It used to wrap to address 0 and write there, which a read of
            // address 0 could then see.
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const -1 i32.const 9 i32.store offset=1))"));
        })
        Test("fill, copy and init refuse a wrapped range", {
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const -1 i32.const 0 i32.const 2 memory.fill 0))"));
            Expect(traps("(module (memory 1) (func (export \"f\")"
                         " i32.const 0 i32.const 0 i32.const -1 memory.copy 0 0))"));
            Expect(traps("(module (memory 1) (data \"ab\") (func (export \"f\")"
                         " i32.const -1 i32.const 0 i32.const 1 memory.init 0 0))"));
            Expect(traps("(module (table 2 funcref) (func (export \"f\")"
                         " i32.const -1 ref.null func i32.const 2 table.fill 0))"));
        })
        Test("an access that does fit is still allowed", {
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 0 i32.const 305419896 i32.store"
                " i32.const 0 i32.load"
                " i32.const 65532 i32.load))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), 305419896);
            ExpectEq(i32_at(results, 1), 0);
        })
        Test("one byte past the end still traps", {
            Expect(traps("(module (memory 1) (func (export \"f\") (result i32)"
                         " i32.const 65533 i32.load))"));
        })
    })
    Category("growing by a count with its top bit set", {
        Test("table.grow refuses instead of shrinking", {
            // It used to answer 1 and leave a two-slot table with one slot.
            std::vector<Value> results = run(
                "(module (table 2 funcref) (func (export \"f\") (result i32 i32)"
                " ref.null func i32.const -1 table.grow 0 table.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 2);
        })
        Test("memory.grow refuses instead of shrinking", {
            // It used to answer 1 and leave a one-page memory with no pages.
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const -1 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
        })
        Test("a 32-bit memory is bounded at 2^16 pages", {
            // The bound applied to both widths was 2^48, which is a 64-bit
            // memory's. A 32-bit one could be asked for 2^32 pages and went
            // to the allocator for 256 TiB, which came back as a bad_alloc
            // rather than as -1.
            std::vector<Value> results = run(
                "(module (memory 1) (func (export \"f\") (result i32 i32)"
                " i32.const 65536 memory.grow 0 memory.size 0))");
            AssertEq(results.size(), 2U);
            ExpectEq(i32_at(results, 0), -1);
            ExpectEq(i32_at(results, 1), 1);
            // One page short of the bound is allowed.
            std::vector<Value> ok = run(
                "(module (memory 1) (func (export \"f\") (result i32)"
                " i32.const 65535 memory.grow 0))");
            AssertEq(ok.size(), 1U);
            ExpectEq(i32_at(ok, 0), 1);
        })
    })
};

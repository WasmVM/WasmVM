// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Execution of try_table catch dispatch.
//
// Per spec, a catch clause's label index is resolved in the context ENCLOSING
// the try_table: index 0 is the innermost enclosing block, never the try_table
// itself (issue #4). Each guest below is built so that the old convention
// (0 = the try_table's own label) lands on different code than the spec one,
// so a regression flips the returned value instead of merely passing.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>
#include <invoke/exception.hpp>

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

} // namespace

Suite invoke_exception {
    Category("catch label targets", {
        Test("catch_all branches to the enclosing block, not the try_table", {
            // Catching at the try_table's own end would fall through to
            // `i32.const 1 return`; branching to $h reaches `i32.const 2`.
            std::vector<Value> r = run(
                "(module (type (func)) (tag $t (type 0)) (func (export \"f\") (result i32) "
                "block $h "
                "  try_table (catch_all $h) throw $t end "
                "  i32.const 1 return "
                "end "
                "i32.const 2))");
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)2);
        })
        Test("catch delivers the payload to the enclosing block's label", {
            // The thrown i32 must arrive at block $h (result i32); catching at
            // the try_table itself would drop it and return the 0 instead.
            std::vector<Value> r = run(
                "(module (type (func (param i32))) (tag $t0 (type 0)) (func (export \"f\") (result i32) "
                "block $h (result i32) "
                "  try_table (catch $t0 $h) i32.const 7 throw $t0 end "
                "  i32.const 0 "
                "end))");
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)7);
        })
    })
};

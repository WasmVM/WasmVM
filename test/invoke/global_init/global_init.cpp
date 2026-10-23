// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// A funcref a global is initialised with must name the function at its
// store address, the same as ref.func at run time and an elem segment.
//
// The global initialiser stored the module's own function index instead.
// The two only agree while the module's functions are the first in the
// store, which is never the case under the wasmvm CLI: the host modules
// register theirs first. The funcref then named whichever function sat at
// that low store address -- a host function -- so a call through it either
// trapped on a type mismatch or ran the wrong function outright.
//
// Each test here puts host functions at the bottom of the store before
// instantiating the module, as the CLI does. They answer 99, so a call that
// lands on one of them instead of the module's own function shows up as 99.

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

std::vector<Value> host_answer(Stack&) {
    return {Value(i32_t(99))};
}

// Instantiate `wat` into a store whose first `host_funcs` functions are host
// functions of type [] -> [i32], then call its exported "f".
i32_t call_f(const std::string& wat, size_t host_funcs) {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store&      store = stores.emplace_back();
    ModuleInst& host  = instances.emplace_back();
    for (size_t i = 0; i < host_funcs; ++i) {
        FuncInst& func = store.funcs.emplace_back(host);
        func.type.results.emplace_back(ValueType::i32);
        func.body = &host_answer;
    }

    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, {});
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            std::vector<Value> results = stack.run();
            return std::get<i32_t>(results.at(0));
        }
    }
    throw Exception::Exception("test module has no exported function 'f'");
}

// $a answers 1 and $b answers 2; the global holds a reference to $b, which
// is the module's function 1.
const std::string module_head =
    "(module (type $r (func (result i32))) "
    "(func $a (type $r) i32.const 1) "
    "(func $b (type $r) i32.const 2) "
    "(global $g funcref (ref.func $b)) ";

const std::string through_call_ref = module_head +
    "(func (export \"f\") (result i32) global.get $g call_ref $r))";

const std::string through_table = module_head +
    "(table 1 funcref) "
    "(func (export \"f\") (result i32) "
    "i32.const 0 global.get $g table.set 0 "
    "i32.const 0 call_indirect (type $r)))";

} // namespace

Suite invoke_global_init {
    Category("a global's ref.func names the function at its store address", {
        Test("call_ref through the global, host functions below the module", {
            ExpectEq(call_f(through_call_ref, 2), (i32_t)2);
        })
        Test("call_indirect after storing the global in a table, host functions below the module", {
            ExpectEq(call_f(through_table, 2), (i32_t)2);
        })
        Test("call_ref through the global, module first in the store", {
            ExpectEq(call_f(through_call_ref, 0), (i32_t)2);
        })
    })
};

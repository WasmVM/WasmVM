// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// #31: with validation skipped (wasmvm --force, or an embedder that does not
// call module_validate), a module may carry an out-of-range function index.
// Every site that reads moduleInst.funcaddrs[index] by a module-supplied index
// must reject it with a thrown Exception instead of indexing the host vector
// out of bounds. The validator rejects such indices, so these modules are
// deliberately only parsed, never validated, which is exactly the --force path.
//
// module_instanciate runs active elem segments and the start function, so the
// instantiation-time sites (elem, global initializer, export) and the runtime
// sites reached from a start body (ref.func, call, return_call) are all driven
// from here. Each bad case asserts the "function index out of range" throw; each
// positive control uses an in-range index and must instantiate cleanly, so the
// throw is the bound rejecting the index, not the module being unusable.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <sstream>
#include <string>

using namespace WasmVM;
using namespace Testing;

namespace {

// Parse WITHOUT module_validate (the --force / skip-validation path), then
// instantiate into a fresh store.
void instantiate_unvalidated(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source); // no module_validate on purpose (#31)
    static std::list<ModuleInst> instances;
    static std::list<Store>      stores;
    Store&      store    = stores.emplace_back();
    ModuleInst& instance = instances.emplace_back();
    module_instanciate(instance, store, module, {});
}

// Whether instantiating `wat` throws the #31 out-of-range rejection.
bool throws_oob(const std::string& wat) {
    try {
        instantiate_unvalidated(wat);
    } catch (Exception::Exception& e) {
        if (std::string(e.what()).find("function index out of range") != std::string::npos) {
            return true;
        }
        std::cout << "      threw, but not the out-of-range message: " << e.what() << std::endl;
        return false;
    }
    std::cout << "      did not throw" << std::endl;
    return false;
}

// Whether instantiating `wat` (an in-range module) completes without throwing.
bool accepts(const std::string& wat) {
    try {
        instantiate_unvalidated(wat);
    } catch (Exception::Exception& e) {
        std::cout << "      unexpected throw: " << e.what() << std::endl;
        return false;
    }
    return true;
}

} // namespace

Suite invoke_force_bounds {
    Category("out-of-range function index throws instead of reading past funcaddrs[] (#31)", {
        Test("element segment ref.func", {
            Expect(throws_oob("(module (func $f) (table 1 funcref) (elem (i32.const 0) func 7))"));
            Expect(accepts("(module (func $f) (table 1 funcref) (elem (i32.const 0) func 0))"));
        })
        Test("global initializer ref.func", {
            Expect(throws_oob("(module (func $f) (global funcref (ref.func 7)))"));
            Expect(accepts("(module (func $f) (global funcref (ref.func 0)))"));
        })
        Test("function export", {
            Expect(throws_oob("(module (func) (export \"x\" (func 7)))"));
            Expect(accepts("(module (func) (export \"x\" (func 0)))"));
        })
        Test("runtime ref.func via start", {
            Expect(throws_oob("(module (func $s ref.func 7 drop) (start $s))"));
            Expect(accepts("(module (func $s ref.func 0 drop) (start $s))"));
        })
        Test("call via start", {
            Expect(throws_oob("(module (func $s call 7) (start $s))"));
            Expect(accepts("(module (func $t) (func $s call 0) (start $s))"));
        })
        Test("return_call via start", {
            Expect(throws_oob("(module (func $s return_call 7) (start $s))"));
            Expect(accepts("(module (func $t) (func $s return_call 0) (start $s))"));
        })
    })
};

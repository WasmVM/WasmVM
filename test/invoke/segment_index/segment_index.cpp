// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Every data and element segment owns an index, whatever its mode (#48).
//
// Instantiation gave a slot in the module's data and element address lists
// only to passive segments. The instructions naming a segment use the
// module's own index, which counts active and declarative segments too, so
// a passive segment placed after one of those was reached through the wrong
// slot: the instruction used another segment, or read past the end of the
// list. An active or declarative segment itself had no slot at all.
//
// The spec runs data.drop / elem.drop on such a segment at instantiation, so
// it keeps its index and is empty from then on: a copy of 0 items from it
// succeeds, a copy of more traps.

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

// Parse, validate and instantiate `wat`, then call its exported "f".
std::vector<Value> run(const std::string& wat) {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
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

i32_t run_i32(const std::string& wat) {
    return std::get<i32_t>(run(wat).at(0));
}

bool traps(const std::string& wat) {
    try {
        run(wat);
    } catch (Exception::Trap&) {
        return true;
    }
    return false;
}

// Data: segment 0 is `head` (active or nothing), then the passive "AAAA"
// and "BBBB". `f` copies passive segment `index` and answers its first byte,
// so the right segment answers 65 ('A') and the shifted one 66 ('B').
std::string data_module(const std::string& head, const std::string& index) {
    return "(module (memory 1) " + head +
        " (data \"AAAA\") (data \"BBBB\")"
        " (func (export \"f\") (result i32)"
        " i32.const 100 i32.const 0 i32.const 4 memory.init 0 " + index +
        " i32.const 100 i32.load8_u))";
}

// Elements: $a answers 1, $b answers 2. Segment 0 is `head`, then passive
// segments holding $a and $b. `f` copies passive segment `index` into table
// slot 1 and calls it, so the right segment answers 1 and the shifted one 2.
std::string elem_module(const std::string& head, const std::string& index) {
    return "(module (type $r (func (result i32))) (table 2 funcref)"
        " (func $a (type $r) i32.const 1) (func $b (type $r) i32.const 2) " + head +
        " (elem funcref (ref.func $a)) (elem funcref (ref.func $b))"
        " (func (export \"f\") (result i32)"
        " i32.const 1 i32.const 0 i32.const 1 table.init 0 " + index +
        " i32.const 1 call_indirect (type $r)))";
}

const std::string active_data = "(data (memory 0) (i32.const 0) \"ZZZZ\")";
const std::string active_elem = "(elem (table 0) (offset i32.const 0) funcref (ref.func $b))";
const std::string declare_elem = "(elem declare funcref (ref.func $b))";

// `f` runs `body` against a module whose only data segment is active.
std::string on_active_data(const std::string& body) {
    return "(module (memory 1) " + active_data + " (func (export \"f\") " + body + "))";
}

// `f` runs `body` against a module whose only element segment is `segment`.
std::string on_elem(const std::string& segment, const std::string& body) {
    return "(module (type $r (func (result i32))) (table 2 funcref)"
        " (func $b (type $r) i32.const 2) " + segment +
        " (func (export \"f\") " + body + "))";
}

} // namespace

Suite segment_index {
    Category("a passive segment after an active or declarative one keeps its index", {
        Test("memory.init behind an active data segment", {
            ExpectEq(run_i32(data_module(active_data, "1")), (i32_t)65);
            ExpectEq(run_i32(data_module(active_data, "2")), (i32_t)66);
        })
        Test("memory.init with only passive segments, for comparison", {
            ExpectEq(run_i32(data_module("", "0")), (i32_t)65);
            ExpectEq(run_i32(data_module("", "1")), (i32_t)66);
        })
        Test("table.init behind an active element segment", {
            ExpectEq(run_i32(elem_module(active_elem, "1")), (i32_t)1);
            ExpectEq(run_i32(elem_module(active_elem, "2")), (i32_t)2);
        })
        Test("table.init behind a declarative element segment", {
            ExpectEq(run_i32(elem_module(declare_elem, "1")), (i32_t)1);
            ExpectEq(run_i32(elem_module(declare_elem, "2")), (i32_t)2);
        })
        Test("array.new_data behind an active data segment", {
            ExpectEq(run_i32(
                "(module (type $bytes (array (mut i8))) (memory 1) " + active_data +
                " (data \"AAAA\") (data \"BBBB\")"
                " (func (export \"f\") (result i32)"
                " i32.const 0 i32.const 4 array.new_data $bytes 1"
                " i32.const 0 array.get_u $bytes))"), (i32_t)65);
        })
        Test("array.new_elem behind an active element segment", {
            ExpectEq(run_i32(
                "(module (type $r (func (result i32))) (type $fns (array (mut funcref)))"
                " (table 2 funcref)"
                " (func $a (type $r) i32.const 1) (func $b (type $r) i32.const 2) " + active_elem +
                " (elem funcref (ref.func $a)) (elem funcref (ref.func $b))"
                " (func (export \"f\") (result i32)"
                " i32.const 1"
                " i32.const 0 i32.const 1 array.new_elem $fns 1"
                " i32.const 0 array.get $fns"
                " table.set 0"
                " i32.const 1 call_indirect (type $r)))"), (i32_t)1);
        })
    })
    Category("an active or declarative segment is empty once instantiated", {
        Test("memory.init of 0 bytes from an active data segment succeeds", {
            Expect(!traps(on_active_data("i32.const 100 i32.const 0 i32.const 0 memory.init 0 0")));
        })
        Test("memory.init of 1 byte from an active data segment traps", {
            Expect(traps(on_active_data("i32.const 100 i32.const 0 i32.const 1 memory.init 0 0")));
        })
        Test("data.drop of an active data segment succeeds", {
            Expect(!traps(on_active_data("data.drop 0")));
        })
        Test("table.init of 0 elements from an active element segment succeeds", {
            Expect(!traps(on_elem(active_elem, "i32.const 1 i32.const 0 i32.const 0 table.init 0 0")));
        })
        Test("table.init of 1 element from an active element segment traps", {
            Expect(traps(on_elem(active_elem, "i32.const 1 i32.const 0 i32.const 1 table.init 0 0")));
        })
        Test("table.init of 0 elements from a declarative element segment succeeds", {
            Expect(!traps(on_elem(declare_elem, "i32.const 1 i32.const 0 i32.const 0 table.init 0 0")));
        })
        Test("table.init of 1 element from a declarative element segment traps", {
            Expect(traps(on_elem(declare_elem, "i32.const 1 i32.const 0 i32.const 1 table.init 0 0")));
        })
        Test("elem.drop of an active or declarative element segment succeeds", {
            Expect(!traps(on_elem(active_elem, "elem.drop 0")));
            Expect(!traps(on_elem(declare_elem, "elem.drop 0")));
        })
    })
};

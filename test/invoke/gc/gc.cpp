// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Executing the struct and array instructions.
//
// All twenty threw Exception::unreachable until now: there was no runtime
// object to allocate and no value type that could hold a reference to one, so
// the interpreter had nothing to do. What is pinned here is the arithmetic
// (field and element order, packed widths, the copy direction), the identity
// (a reference is a handle, not a copy), and the three trapping conditions the
// spec names: a null reference, an out-of-range index, and a segment read that
// runs past its end.

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

// Parse, validate, instantiate, and call the exported "f".
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

// $s: struct { i32 $x, mut i8 $y, mut i64 $z }
// $a: array (mut i32);  $p: array (mut i8);  $w: array (mut i64)
// $r: array (mut (ref null $s));  $e: array (mut funcref)
// data 0 is 01 02 03 04; elem 0 holds one funcref.
const std::string prelude =
    "(module"
    " (type $s (sub (struct (field $x i32) (field $y (mut i8)) (field $z (mut i64)))))"
    " (type $a (array (mut i32)))"
    " (type $p (array (mut i8)))"
    " (type $w (array (mut i64)))"
    " (type $r (array (mut (ref null $s))))"
    " (type $e (array (mut funcref)))"
    " (data \"\\01\\02\\03\\04\")"
    " (func $g)"
    " (elem func $g)";

std::string body(const std::string& results, const std::string& code) {
    return prelude + " (func (export \"f\") " + results + " " + code + "))";
}

i32_t i32_at(const std::vector<Value>& values, size_t index) {
    return std::get<i32_t>(values.at(index));
}
i64_t i64_at(const std::vector<Value>& values, size_t index) {
    return std::get<i64_t>(values.at(index));
}

// Whether running `code` traps.
bool traps(const std::string& results, const std::string& code) {
    try {
        run(body(results, code));
    } catch (Exception::Trap&) {
        return true;
    } catch (Exception::Exception& e) {
        std::cout << "      not a trap: " << e.what() << std::endl;
        return false;
    }
    std::cout << "      did not trap" << std::endl;
    return false;
}

// $t is [] -> [] and $u is [i32] -> []; $f has type $t and $g type $u.
std::string funcs_module(const std::string& results, const std::string& code) {
    return "(module (type $t (func)) (type $u (func (param i32)))"
           " (func $f (type $t)) (func $g (type $u))"
           " (func (export \"f\") " + results + " " + code + "))";
}

// A non-null externref cannot be built by any wasm instruction (externref_t is
// void* and nothing constructs one), so it can only enter from the host. These
// tests import a global holding a real non-null externref and read it with
// global.get $ext inside `code`; that is the only way to drive a non-null value
// into the externref branch of ref_matches_heaptype (#40). The (ref extern)
// positive controls confirm the value really is a non-null externref and the
// branch is reached, so a 0/trap/no-branch for noextern is not a false pass.
int extern_dummy = 0;
std::vector<Value> run_extern(const std::string& results, const std::string& code) {
    static std::list<ModuleInst> instances;
    static std::list<Store>      stores;
    Store& store = stores.emplace_back();
    index_t gaddr = store.globals.size();
    store.globals.push_back(GlobalInst{ValueType::externref, Value(externref_t(&extern_dummy))});
    const std::string wat =
        "(module (import \"a\" \"ext\" (global $ext externref))"
        " (func (export \"f\") " + results + " " + code + "))";
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    ModuleInst& instance = instances.emplace_back();
    std::vector<ExternVal> imports = { ExternVal{ ExternVal::ExternType::Global, gaddr } };
    module_instanciate(instance, store, module, imports);
    for (ExportInst& exported : instance.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            return stack.run();
        }
    }
    throw Exception::Exception("extern test module has no exported function 'f'");
}

} // namespace

Suite invoke_gc {
    Category("struct", {
        Test("fields come off the stack in declaration order", {
            // 42 is $x, 7 is $y, 9 is $z: the last operand is the last field.
            std::vector<Value> r = run(body("(result i32 i64)",
                "(local $o (ref $s))"
                " i32.const 42 i32.const 7 i64.const 9 struct.new $s local.set $o"
                " local.get $o struct.get $s $x local.get $o struct.get $s $z"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 42);
            ExpectEq(i64_at(r, 1), 9);
        })
        Test("struct.new_default zeroes every field", {
            std::vector<Value> r = run(body("(result i32 i32 i64)",
                "(local $o (ref $s)) struct.new_default $s local.set $o"
                " local.get $o struct.get $s $x"
                " local.get $o struct.get_u $s $y"
                " local.get $o struct.get $s $z"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 0);
            ExpectEq(i32_at(r, 1), 0);
            ExpectEq(i64_at(r, 2), 0);
        })
        Test("a packed field keeps its own width", {
            // 200 stored in an i8: unsigned it reads back 200, signed -56, and
            // the bits above the eighth are gone.
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $o (ref $s))"
                " i32.const 0 i32.const 456 i64.const 0 struct.new $s local.set $o"
                " local.get $o struct.get_u $s $y local.get $o struct.get_s $s $y"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 200);   // 456 & 0xff
            ExpectEq(i32_at(r, 1), -56);
        })
        Test("struct.set writes through the reference", {
            std::vector<Value> r = run(body("(result i64)",
                "(local $o (ref $s)) i32.const 1 i32.const 2 i64.const 3 struct.new $s local.set $o"
                " local.get $o i64.const 77 struct.set $s $z"
                " local.get $o struct.get $s $z"));
            AssertEq(r.size(), 1U);
            ExpectEq(i64_at(r, 0), 77);
        })
        Test("two references name the same struct", {
            std::vector<Value> r = run(body("(result i64)",
                "(local $o (ref $s)) (local $p (ref $s))"
                " i32.const 1 i32.const 2 i64.const 3 struct.new $s local.set $o"
                " local.get $o local.set $p"
                " local.get $o i64.const 55 struct.set $s $z"
                " local.get $p struct.get $s $z"));
            AssertEq(r.size(), 1U);
            ExpectEq(i64_at(r, 0), 55);
        })
        Test("two struct.new calls do not", {
            std::vector<Value> r = run(body("(result i64)",
                "(local $o (ref $s)) (local $p (ref $s))"
                " i32.const 1 i32.const 2 i64.const 3 struct.new $s local.set $o"
                " i32.const 1 i32.const 2 i64.const 3 struct.new $s local.set $p"
                " local.get $o i64.const 55 struct.set $s $z"
                " local.get $p struct.get $s $z"));
            AssertEq(r.size(), 1U);
            ExpectEq(i64_at(r, 0), 3);
        })
        Test("reading through a null reference traps", {
            Expect(traps("(result i32)", "ref.null $s struct.get $s $x"));
        })
        Test("writing through a null reference traps", {
            Expect(traps("", "ref.null $s i64.const 1 struct.set $s $z"));
        })
    })
    Category("array", {
        Test("array.new fills with the given value", {
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $o (ref $a)) i32.const 5 i32.const 3 array.new $a local.set $o"
                " local.get $o array.len"
                " local.get $o i32.const 0 array.get $a"
                " local.get $o i32.const 2 array.get $a"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 3);
            ExpectEq(i32_at(r, 1), 5);
            ExpectEq(i32_at(r, 2), 5);
        })
        Test("array.new_fixed keeps the operand order", {
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $o (ref $a))"
                " i32.const 10 i32.const 20 i32.const 30 array.new_fixed $a 3 local.set $o"
                " local.get $o i32.const 0 array.get $a"
                " local.get $o i32.const 1 array.get $a"
                " local.get $o i32.const 2 array.get $a"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 10);
            ExpectEq(i32_at(r, 1), 20);
            ExpectEq(i32_at(r, 2), 30);
        })
        Test("array.set writes one element", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $o (ref $a)) i32.const 0 i32.const 3 array.new $a local.set $o"
                " local.get $o i32.const 1 i32.const 9 array.set $a"
                " local.get $o i32.const 1 array.get $a"
                " local.get $o i32.const 2 array.get $a"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 9);
            ExpectEq(i32_at(r, 1), 0);
        })
        Test("array.fill covers exactly its range", {
            std::vector<Value> r = run(body("(result i32 i32 i32 i32)",
                "(local $o (ref $a)) i32.const 0 i32.const 4 array.new $a local.set $o"
                " local.get $o i32.const 1 i32.const 8 i32.const 2 array.fill $a"
                " local.get $o i32.const 0 array.get $a"
                " local.get $o i32.const 1 array.get $a"
                " local.get $o i32.const 2 array.get $a"
                " local.get $o i32.const 3 array.get $a"));
            AssertEq(r.size(), 4U);
            ExpectEq(i32_at(r, 0), 0);
            ExpectEq(i32_at(r, 1), 8);
            ExpectEq(i32_at(r, 2), 8);
            ExpectEq(i32_at(r, 3), 0);
        })
        Test("a packed element keeps its own width", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $o (ref $p)) i32.const 456 i32.const 2 array.new $p local.set $o"
                " local.get $o i32.const 0 array.get_u $p"
                " local.get $o i32.const 0 array.get_s $p"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 200);
            ExpectEq(i32_at(r, 1), -56);
        })
        Test("an out-of-range index traps, at the end as well", {
            Expect(traps("(result i32)", "i32.const 0 i32.const 3 array.new $a i32.const 3 array.get $a"));
            Expect(traps("", "i32.const 0 i32.const 3 array.new $a i32.const 3 i32.const 1 array.set $a"));
            // -1 as an index is 0xffffffff, not a step back from the end.
            Expect(traps("(result i32)", "i32.const 0 i32.const 3 array.new $a i32.const -1 array.get $a"));
        })
        Test("array.len and the accessors trap on null", {
            Expect(traps("(result i32)", "ref.null $a array.len"));
            Expect(traps("(result i32)", "ref.null $a i32.const 0 array.get $a"));
            Expect(traps("", "ref.null $a i32.const 0 i32.const 1 array.set $a"));
            Expect(traps("", "ref.null $a i32.const 0 i32.const 0 i32.const 0 array.fill $a"));
        })
        Test("a fill or copy that runs off the end traps", {
            Expect(traps("", "i32.const 0 i32.const 3 array.new $a"
                             " i32.const 2 i32.const 1 i32.const 2 array.fill $a"));
            Expect(traps("", "i32.const 0 i32.const 3 array.new $a i32.const 2"
                             " i32.const 0 i32.const 3 array.new $a i32.const 0 i32.const 2 array.copy $a $a"));
        })
    })
    Category("array.copy", {
        Test("copies forwards between two arrays", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $src (ref $a)) (local $dst (ref $a))"
                " i32.const 10 i32.const 3 array.new $a local.set $src"
                " i32.const 0 i32.const 3 array.new $a local.set $dst"
                " local.get $dst i32.const 1 local.get $src i32.const 0 i32.const 2 array.copy $a $a"
                " local.get $dst i32.const 0 array.get $a"
                " local.get $dst i32.const 1 array.get $a"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 0);
            ExpectEq(i32_at(r, 1), 10);
        })
        Test("overlapping in one array does not eat its own source", {
            // [1,2,0,0] copied from 0 to 1 for 2 elements is [1,1,2,0]: the
            // second element read must be the original 2, not the 1 just
            // written over it.
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $o (ref $a)) i32.const 0 i32.const 4 array.new $a local.set $o"
                " local.get $o i32.const 0 i32.const 1 array.set $a"
                " local.get $o i32.const 1 i32.const 2 array.set $a"
                " local.get $o i32.const 1 local.get $o i32.const 0 i32.const 2 array.copy $a $a"
                " local.get $o i32.const 1 array.get $a"
                " local.get $o i32.const 2 array.get $a"
                " local.get $o i32.const 3 array.get $a"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 2);
            ExpectEq(i32_at(r, 2), 0);
        })
        Test("a null array on either side traps", {
            Expect(traps("", "ref.null $a i32.const 0"
                             " i32.const 0 i32.const 1 array.new $a i32.const 0 i32.const 0 array.copy $a $a"));
            Expect(traps("", "i32.const 0 i32.const 1 array.new $a i32.const 0"
                             " ref.null $a i32.const 0 i32.const 0 array.copy $a $a"));
        })
    })
    Category("from a data or elem segment", {
        Test("array.new_data reads bytes at their own width", {
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $o (ref $p)) i32.const 1 i32.const 3 array.new_data $p 0 local.set $o"
                " local.get $o array.len"
                " local.get $o i32.const 0 array.get_u $p"
                " local.get $o i32.const 2 array.get_u $p"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 3);
            ExpectEq(i32_at(r, 1), 2); // data is 01 02 03 04, from offset 1
            ExpectEq(i32_at(r, 2), 4);
        })
        Test("a wider element takes more bytes", {
            // Four bytes 01 02 03 04 little-endian make one i64 only if eight
            // are asked for, so one element of an i64 array overruns them.
            Expect(traps("(result i32)", "i32.const 0 i32.const 1 array.new_data $w 0 array.len"));
        })
        Test("array.init_data writes into an existing array", {
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $o (ref $p)) i32.const 5 array.new_default $p local.set $o"
                " local.get $o i32.const 2 i32.const 0 i32.const 2 array.init_data $p 0"
                " local.get $o i32.const 1 array.get_u $p"
                " local.get $o i32.const 2 array.get_u $p"
                " local.get $o i32.const 3 array.get_u $p"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 0);
            ExpectEq(i32_at(r, 1), 1);
            ExpectEq(i32_at(r, 2), 2);
        })
        Test("reading past the end of a segment traps", {
            Expect(traps("(result i32)", "i32.const 2 i32.const 3 array.new_data $p 0 array.len"));
            Expect(traps("", "i32.const 5 array.new_default $p"
                             " i32.const 0 i32.const 3 i32.const 2 array.init_data $p 0"));
        })
        Test("array.new_elem and array.init_elem carry references", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $o (ref $e)) i32.const 0 i32.const 1 array.new_elem $e 0 local.set $o"
                " local.get $o array.len"
                " local.get $o i32.const 0 array.get $e ref.is_null"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 0); // the segment's funcref is not null
        })
        Test("reading past the end of an elem segment traps", {
            Expect(traps("(result i32)", "i32.const 0 i32.const 2 array.new_elem $e 0 array.len"));
        })
    })
    Category("references as ordinary values", {
        Test("an array of struct references starts null and can be filled", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "(local $o (ref $r))"
                " i32.const 2 array.new_default $r local.set $o"
                " local.get $o i32.const 0 array.get $r ref.is_null"
                " local.get $o i32.const 0 i32.const 1 i32.const 2 i64.const 3 struct.new $s array.set $r"
                " local.get $o i32.const 0 array.get $r ref.is_null"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 0);
        })
        Test("a struct reference survives being returned", {
            std::vector<Value> r = run(body("(result (ref $s))",
                "i32.const 1 i32.const 2 i64.const 3 struct.new $s"));
            AssertEq(r.size(), 1U);
            Assert(std::holds_alternative<structref_t>(r[0]));
            Assert(std::get<structref_t>(r[0]) != nullptr);
            AssertEq(std::get<structref_t>(r[0])->fields.size(), 3U);
            ExpectEq(std::get<i32_t>(std::get<structref_t>(r[0])->fields[0]), 1);
        })
        Test("ref.null of a concrete type is null", {
            std::vector<Value> r = run(body("(result i32 i32)",
                "ref.null $s ref.is_null ref.null $a ref.is_null"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 1);
        })
    })
    // A cast is how an abstract reference becomes a concrete one. Both ends
    // matter: the validator has to refine the type so the field access is
    // allowed at all, and the interpreter has to trap when the object is not
    // what the cast claims.
    Category("casts", {
        Test("build, widen to anyref, cast back, read the field", {
            std::vector<Value> r = run(body("(result i32)",
                "(local $any anyref)"
                " i32.const 7 i32.const 0 i64.const 0 struct.new $s local.set $any"
                " local.get $any ref.cast (ref $s) struct.get $s $x"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 7);
        })
        Test("ref.test answers for the right type and the wrong one", {
            std::vector<Value> r = run(body("(result i32 i32 i32)",
                "(local $any anyref)"
                " i32.const 3 array.new_default $a local.set $any"
                " local.get $any ref.test (ref $a)"
                " local.get $any ref.test (ref $s)"
                " local.get $any ref.test (ref array)"));
            AssertEq(r.size(), 3U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 0);
            ExpectEq(i32_at(r, 2), 1);
        })
        Test("a cast to the wrong type traps", {
            Expect(traps("(result i32)",
                "(local $any anyref)"
                " i32.const 3 array.new_default $a local.set $any"
                " local.get $any ref.cast (ref $s) struct.get $s $x"));
        })
        Test("a non-null cast of a null reference traps", {
            Expect(traps("(result i32)",
                "ref.null $s ref.cast (ref $s) struct.get $s $x"));
        })
        Test("br_on_cast branches on a match and falls through otherwise", {
            // The array goes to the fall-through, the struct to the label.
            const std::string code =
                "(local $any anyref)"
                " i32.const 3 array.new_default $a local.set $any"
                " block $h (result (ref $s)) local.get $any"
                "   br_on_cast $h (ref null any) (ref $s)"
                "   drop i32.const 111 return end"
                " struct.get $s $x";
            std::vector<Value> r = run(body("(result i32)", code));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 111); // fell through: it was an array
        })
    })
    // A function reference against a concrete function type. The runtime
    // checked a funcref only against the abstract func, so every cast to a
    // concrete function type trapped and every test answered 0. A function
    // matches when its type has the same parameters and results, the same
    // check call_ref makes; declared supertypes are not followed (#38).
    Category("casts of function references", {
        Test("ref.cast to the function's own type succeeds", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "ref.func $f ref.cast (ref $t) drop i32.const 1"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 1);
        })
        Test("ref.cast to a different function type traps", {
            ExpectThrows(Exception::Trap, run(funcs_module("",
                "ref.func $g ref.cast (ref $t) drop")));
        })
        Test("ref.test answers 1 for the function's type and 0 for another", {
            std::vector<Value> r = run(funcs_module("(result i32 i32)",
                "ref.func $f ref.test (ref $t) ref.func $g ref.test (ref $t)"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 1);
            ExpectEq(i32_at(r, 1), 0);
        })
        Test("br_on_cast branches for the function's type", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "block $h (result (ref $t)) ref.func $f"
                "   br_on_cast $h (ref null func) (ref $t)"
                "   drop i32.const 111 return end"
                " drop i32.const 222"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 222); // branched: $f is a $t
        })
    })
    // #40: nofunc is the bottom of the function hierarchy and holds only null,
    // so a non-null funcref must not match it. The matching (ref func) controls
    // use the same funcref, so each op is shown to answer both ways and the
    // negative result is not a check that silently never ran.
    Category("non-null funcref against the bottom type nofunc (#40)", {
        Test("ref.test is 0 for nofunc and 1 for func", {
            std::vector<Value> r = run(funcs_module("(result i32 i32)",
                "ref.func $f ref.test (ref nofunc) ref.func $f ref.test (ref func)"));
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 0); // nofunc: no match
            ExpectEq(i32_at(r, 1), 1); // func: matches
        })
        Test("ref.cast traps for nofunc", {
            ExpectThrows(Exception::Trap, run(funcs_module("",
                "ref.func $f ref.cast (ref nofunc) drop")));
        })
        Test("ref.cast succeeds for func", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "ref.func $f ref.cast (ref func) drop i32.const 1"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 1);
        })
        Test("br_on_cast to nofunc does not branch", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "block $h (result (ref nofunc)) ref.func $f"
                "   br_on_cast $h (ref null func) (ref nofunc)"
                "   drop i32.const 111 return end"
                " drop i32.const 222"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 111); // fell through: $f is not a nofunc
        })
        Test("br_on_cast to func branches", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "block $h (result (ref func)) ref.func $f"
                "   br_on_cast $h (ref null func) (ref func)"
                "   drop i32.const 111 return end"
                " drop i32.const 222"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 222); // branched: $f is a func
        })
        Test("br_on_cast_fail to nofunc branches", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "block $h (result (ref null func)) ref.func $f"
                "   br_on_cast_fail $h (ref null func) (ref nofunc)"
                "   drop i32.const 111 return end"
                " drop i32.const 222"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 222); // branched: $f fails the nofunc cast
        })
        Test("br_on_cast_fail to func does not branch", {
            std::vector<Value> r = run(funcs_module("(result i32)",
                "block $h (result (ref null func)) ref.func $f"
                "   br_on_cast_fail $h (ref null func) (ref func)"
                "   drop i32.const 111 return end"
                " drop i32.const 222"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 111); // fell through: $f passes the func cast
        })
    })
    // #40: noextern is the bottom of the extern hierarchy (null only), so a
    // non-null externref must not match it. See run_extern for why the value
    // comes from a host-imported global; the (ref extern) controls confirm the
    // externref branch is actually reached with a non-null value.
    Category("non-null externref against the bottom type noextern (#40)", {
        Test("ref.test is 0 for noextern and 1 for extern", {
            std::vector<Value> r = run_extern("(result i32 i32)",
                "global.get $ext ref.test (ref noextern)"
                " global.get $ext ref.test (ref extern)");
            AssertEq(r.size(), 2U);
            ExpectEq(i32_at(r, 0), 0); // noextern: no match
            ExpectEq(i32_at(r, 1), 1); // extern: matches
        })
        Test("ref.cast traps for noextern", {
            ExpectThrows(Exception::Trap, run_extern("",
                "global.get $ext ref.cast (ref noextern) drop"));
        })
        Test("ref.cast succeeds for extern", {
            std::vector<Value> r = run_extern("(result i32)",
                "global.get $ext ref.cast (ref extern) drop i32.const 1");
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 1);
        })
        Test("br_on_cast to noextern does not branch", {
            std::vector<Value> r = run_extern("(result i32)",
                "block $h (result (ref noextern)) global.get $ext"
                "   br_on_cast $h (ref null extern) (ref noextern)"
                "   drop i32.const 111 return end"
                " drop i32.const 222");
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 111); // fell through: not a noextern
        })
        Test("br_on_cast to extern branches", {
            std::vector<Value> r = run_extern("(result i32)",
                "block $h (result (ref extern)) global.get $ext"
                "   br_on_cast $h (ref null extern) (ref extern)"
                "   drop i32.const 111 return end"
                " drop i32.const 222");
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 222); // branched: it is an extern
        })
        Test("br_on_cast_fail to noextern branches", {
            std::vector<Value> r = run_extern("(result i32)",
                "block $h (result (ref null extern)) global.get $ext"
                "   br_on_cast_fail $h (ref null extern) (ref noextern)"
                "   drop i32.const 111 return end"
                " drop i32.const 222");
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 222); // branched: fails the noextern cast
        })
        Test("br_on_cast_fail to extern does not branch", {
            std::vector<Value> r = run_extern("(result i32)",
                "block $h (result (ref null extern)) global.get $ext"
                "   br_on_cast_fail $h (ref null extern) (ref extern)"
                "   drop i32.const 111 return end"
                " drop i32.const 222");
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 111); // fell through: passes the extern cast
        })
    })
};

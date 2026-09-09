// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Validation of the GC type definitions and of the struct / array
// instructions against them.
//
// Until now the validator had no case for any struct.* or array.* opcode:
// they fell into the default no-op, so struct.get $s 5 on a one-field struct
// "validated", and so did every GC instruction in a module with no types at
// all. The rules below are the spec's, with one deliberate gap: a reference
// to a struct or array type exists only on the operand stack. ValueType has
// no (ref $t) yet, so no local, global, parameter or result can hold one --
// and the last category pins that a function cannot return one either.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <optional>
#include <sstream>
#include <string>

using namespace WasmVM;
using namespace Testing;

namespace {

std::optional<Exception::Exception> validate(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    return module_validate(module);
}

// True when validation fails and the message names the rule that fired, so a
// module cannot pass this check by being rejected for some other reason.
bool rejected_with(const std::string& wat, const std::string& fragment) {
    std::optional<Exception::Exception> error = validate(wat);
    if (!error) {
        std::cout << "      accepted, expected rejection mentioning '" << fragment << "'\n";
        return false;
    }
    if (std::string(error->what()).find(fragment) == std::string::npos) {
        std::cout << "      rejected for another reason: " << error->what() << "\n";
        return false;
    }
    return true;
}

// $s: struct { i32, mut i8 }; $a: array (mut i32); $p: array (mut i16);
// $c: array i32 (immutable); $b: array (mut i8) with data "abc";
// $f: array (mut funcref) with a passive funcref elem segment.
const std::string prelude =
    "(module"
    " (type $s (struct (field $x i32) (field $y (mut i8))))"
    " (type $a (array (mut i32)))"
    " (type $p (array (mut i16)))"
    " (type $c (array i32))"
    " (type $b (array (mut i8)))"
    " (type $f (array (mut funcref)))"
    " (data \"abc\")"
    " (elem func)";

std::string with_body(const std::string& body) {
    return prelude + " (func " + body + "))";
}

} // namespace

Suite validate_gc_types {
    Category("struct instructions accept", {
        Test("struct.new with the fields in order, a packed field through i32", {
            Expect(!validate(with_body("i32.const 1 i32.const 2 struct.new $s drop")));
        })
        Test("struct.new_default", {
            Expect(!validate(with_body("struct.new_default $s drop")));
        })
        Test("struct.get on a plain field", {
            Expect(!validate(with_body("struct.new_default $s struct.get $s $x drop")));
        })
        Test("struct.get_s and struct.get_u on a packed field", {
            Expect(!validate(with_body("struct.new_default $s struct.get_s $s $y drop")));
            Expect(!validate(with_body("struct.new_default $s struct.get_u $s $y drop")));
        })
        Test("struct.set on a mutable field", {
            Expect(!validate(with_body("struct.new_default $s i32.const 9 struct.set $s $y")));
        })
    })
    Category("struct instructions reject", {
        Test("a wrong operand type for a field", {
            Expect(rejected_with(with_body("i64.const 1 i32.const 2 struct.new $s drop"), "value type not match"));
        })
        Test("too few operands", {
            Expect(rejected_with(with_body("i32.const 2 struct.new $s drop"), "empty validate value stack"));
        })
        Test("a type index that is not a struct", {
            Expect(rejected_with(with_body("struct.new_default $a drop"), "not a struct type for struct.new_default"));
        })
        Test("a type index that does not exist", {
            // Type 0 exists: it is the function's own. 7 does not.
            Expect(rejected_with("(module (func struct.new 7 drop))", "type index not found for struct.new"));
        })
        Test("a field index out of range", {
            Expect(rejected_with(with_body("struct.new_default $s struct.get $s 2 drop"), "field index not found for struct.get"));
        })
        Test("struct.get on a packed field", {
            Expect(rejected_with(with_body("struct.new_default $s struct.get $s $y drop"), "use the _s or _u form"));
        })
        Test("struct.get_s on a plain field", {
            Expect(rejected_with(with_body("struct.new_default $s struct.get_s $s $x drop"), "needs a packed field"));
        })
        Test("struct.set on an immutable field", {
            Expect(rejected_with(with_body("struct.new_default $s i32.const 9 struct.set $s $x"), "immutable field"));
        })
        Test("struct.get without a reference operand", {
            Expect(rejected_with(with_body("i32.const 1 struct.get $s $x drop"), "value type not match"));
        })
        Test("struct.get on a reference to another type", {
            Expect(rejected_with(with_body("i32.const 3 array.new_default $a struct.get $s $x drop"), "reference to a different type"));
        })
    })
    Category("array instructions accept", {
        Test("array.new, array.new_default, array.new_fixed", {
            Expect(!validate(with_body("i32.const 7 i32.const 3 array.new $a drop")));
            Expect(!validate(with_body("i32.const 3 array.new_default $a drop")));
            Expect(!validate(with_body("i32.const 1 i32.const 2 array.new_fixed $a 2 drop")));
        })
        Test("array.get and array.set", {
            Expect(!validate(with_body("i32.const 3 array.new_default $a i32.const 0 array.get $a drop")));
            Expect(!validate(with_body("i32.const 3 array.new_default $a i32.const 0 i32.const 5 array.set $a")));
        })
        Test("array.get_s on a packed element", {
            Expect(!validate(with_body("i32.const 3 array.new_default $p i32.const 0 array.get_s $p drop")));
        })
        Test("array.len", {
            Expect(!validate(with_body("i32.const 3 array.new_default $a array.len drop")));
        })
        Test("array.fill", {
            Expect(!validate(with_body("i32.const 3 array.new_default $a i32.const 0 i32.const 5 i32.const 1 array.fill $a")));
        })
        Test("array.copy between arrays of the same element type", {
            Expect(!validate(with_body(
                "i32.const 3 array.new_default $a i32.const 0"
                " i32.const 3 array.new_default $a i32.const 0 i32.const 1 array.copy $a $a")));
        })
        Test("array.new_data and array.init_data on a packed element", {
            Expect(!validate(with_body("i32.const 0 i32.const 3 array.new_data $b 0 drop")));
            Expect(!validate(with_body("i32.const 3 array.new_default $b i32.const 0 i32.const 0 i32.const 3 array.init_data $b 0")));
        })
        Test("array.new_elem and array.init_elem on a funcref element", {
            Expect(!validate(with_body("i32.const 0 i32.const 0 array.new_elem $f 0 drop")));
            Expect(!validate(with_body("i32.const 3 array.new_default $f i32.const 0 i32.const 0 i32.const 0 array.init_elem $f 0")));
        })
    })
    Category("array instructions reject", {
        Test("a type index that is not an array", {
            Expect(rejected_with(with_body("i32.const 3 array.new_default $s drop"), "not an array type for array.new_default"));
        })
        Test("array.set on an immutable array", {
            Expect(rejected_with(with_body("i32.const 3 array.new_default $c i32.const 0 i32.const 5 array.set $c"), "immutable array"));
        })
        Test("array.get on a packed element", {
            Expect(rejected_with(with_body("i32.const 3 array.new_default $p i32.const 0 array.get $p drop"), "use the _s or _u form"));
        })
        Test("array.len on a non-reference", {
            Expect(rejected_with(with_body("i32.const 3 array.len drop"), "needs an array reference"));
        })
        Test("array.len on a struct reference", {
            Expect(rejected_with(with_body("struct.new_default $s array.len drop"), "needs an array reference"));
        })
        Test("array.copy between different element types", {
            Expect(rejected_with(with_body(
                "i32.const 3 array.new_default $a i32.const 0"
                " i32.const 3 array.new_default $p i32.const 0 i32.const 1 array.copy $a $p"), "different element types"));
        })
        Test("array.new_data on a reference element", {
            Expect(rejected_with(with_body("i32.const 0 i32.const 0 array.new_data $f 0 drop"), "numeric or packed element type"));
        })
        Test("array.new_elem on a numeric element", {
            Expect(rejected_with(with_body("i32.const 0 i32.const 0 array.new_elem $a 0 drop"), "reference element type"));
        })
        Test("a data or elem index that does not exist", {
            Expect(rejected_with(with_body("i32.const 0 i32.const 3 array.new_data $b 1 drop"), "data index not found"));
            Expect(rejected_with(with_body("i32.const 0 i32.const 0 array.new_elem $f 1 drop"), "elem index not found"));
        })
        Test("array.new_fixed with too few operands", {
            Expect(rejected_with(with_body("i32.const 1 array.new_fixed $a 2 drop"), "empty validate value stack"));
        })
    })
    Category("a reference to a GC type stays on the operand stack", {
        Test("it cannot be returned", {
            Expect(rejected_with(prelude + " (func (result i32) struct.new_default $s))", "value type not match"));
        })
        Test("it cannot be stored in a local", {
            Expect(rejected_with(with_body("(local i32) struct.new_default $s local.set 0"), "value type not match"));
        })
        Test("it is a reference for ref.is_null", {
            Expect(!validate(with_body("struct.new_default $s ref.is_null drop")));
        })
        Test("it is not numeric for select", {
            Expect(rejected_with(with_body("struct.new_default $s struct.new_default $s i32.const 1 select drop"), "numeric types"));
        })
    })
    Category("the type section", {
        Test("a subtype chain is accepted", {
            Expect(!validate("(module (type $a (sub (struct (field i32)))) (type $b (sub $a (struct (field i32) (field i64))))"
                             " (type (sub final $b (struct (field i32) (field i64) (field f32)))))"));
        })
        Test("a rec group with a backward supertype is accepted", {
            Expect(!validate("(module (rec (type $a (sub (struct))) (type $b (sub $a (struct)))))"));
        })
        Test("two supertypes are rejected", {
            Expect(rejected_with("(module (type $a (sub (struct))) (type $b (sub (struct))) (type (sub $a $b (struct))))",
                                 "at most one supertype"));
        })
        Test("a supertype defined later is rejected, also inside a rec group", {
            Expect(rejected_with("(module (rec (type $a (sub $b (struct))) (type $b (sub (struct)))))",
                                 "must be defined before"));
        })
        Test("a final supertype is rejected", {
            Expect(rejected_with("(module (type $a (struct)) (type (sub $a (struct))))", "is final"));
        })
        Test("a supertype of another kind is rejected", {
            Expect(rejected_with("(module (type $a (sub (func))) (type (sub $a (struct))))", "different kind"));
        })
    })
    Category("a struct or array type where a function type is required", {
        Test("as a function's type", {
            Expect(rejected_with("(module (type $s (struct)) (func (type $s)))", "func [0]: type index 0 is not a function type"));
        })
        Test("as a block type", {
            Expect(rejected_with("(module (type $s (struct)) (func block (type $s) end))", "not a function type for block type"));
        })
        Test("as a call_indirect type", {
            Expect(rejected_with("(module (type $s (struct)) (table 1 funcref) (func i32.const 0 call_indirect (type $s)))",
                                 "not a function type for call_indirect"));
        })
        Test("as a tag's type", {
            Expect(rejected_with("(module (type $s (struct)) (tag (type $s)))", "tag [0]: type index 0 is not a function type"));
        })
    })
};

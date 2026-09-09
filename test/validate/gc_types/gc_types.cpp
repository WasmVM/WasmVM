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
    " (type $s (sub (struct (field $x i32) (field $y (mut i8)))))"
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
    // A reference to a struct or array type is an ordinary value type now:
    // (ref null? $t) can be a parameter, a result, a local, a global, a field
    // and an array element. Until the value types carried a type index it
    // could only sit on the operand stack, which made these instructions
    // impossible to use for anything.
    Category("a reference to a GC type is an ordinary value", {
        Test("it can be returned", {
            Expect(!validate(prelude + " (func (result (ref $s)) struct.new_default $s))"));
        })
        Test("it can be a parameter and be read through", {
            Expect(!validate(prelude + " (func (param (ref $s)) (result i32) local.get 0 struct.get $s $x))"));
        })
        Test("it can be stored in a local", {
            Expect(!validate(with_body("(local (ref null $s)) struct.new_default $s local.set 0")));
        })
        Test("it can be a global", {
            Expect(!validate(prelude + " (global (ref null $s) (ref.null $s)))"));
        })
        Test("it can be a struct field and an array element", {
            Expect(!validate(
                "(module (type $n (sub (struct (field (mut (ref null $n))))))"
                " (type $l (array (mut (ref null $n))))"
                " (func (result (ref $l)) i32.const 4 array.new_default $l))"));
        })
        Test("a wrong concrete type is still refused", {
            Expect(rejected_with(prelude + " (func (result (ref $a)) struct.new_default $s))",
                                 "reference to a different type"));
        })
        Test("it is a reference for ref.is_null", {
            Expect(!validate(with_body("struct.new_default $s ref.is_null drop")));
        })
        Test("it is not numeric for select", {
            Expect(rejected_with(with_body("struct.new_default $s struct.new_default $s i32.const 1 select drop"), "numeric types"));
        })
    })
    // The subtyping relation, which the type section's declarations and every
    // typed operand are now checked against.
    Category("subtyping", {
        Test("a subtype is accepted where its supertype is expected", {
            Expect(!validate(
                "(module (type $b (sub (struct (field i32)))) (type $d (sub $b (struct (field i32) (field i64))))"
                " (func (param (ref $d)) (result i32) local.get 0 struct.get $b 0))"));
        })
        Test("a supertype is not accepted where the subtype is expected", {
            Expect(rejected_with(
                "(module (type $b (sub (struct (field i32)))) (type $d (sub $b (struct (field i32) (field i64))))"
                " (func (param (ref $b)) (result i64) local.get 0 struct.get $d 1))",
                "reference to a different type"));
        })
        Test("an abstract reference is not accepted where a concrete one is expected", {
            Expect(rejected_with(prelude + " (func (param structref) (result i32) local.get 0 struct.get $s $x))",
                                 "reference to a different type"));
        })
        Test("a concrete reference is accepted where its abstract one is expected", {
            Expect(!validate(prelude + " (func (param (ref $s)) (result i32) local.get 0 array.len))") == false);
            Expect(!validate(prelude + " (func (param (ref $a)) (result i32) local.get 0 array.len))"));
        })
        Test("a non-null reference satisfies a nullable expectation", {
            Expect(!validate(prelude + " (func (param (ref $s)) (result i32) local.get 0 struct.get $s $x))"));
        })
        Test("a nullable reference does not satisfy a non-null one", {
            Expect(rejected_with(
                prelude + " (func (param (ref null $s)) (result (ref $s)) local.get 0))",
                "nullable reference where a non-null one is required"));
        })
        Test("the func and any hierarchies do not mix", {
            Expect(rejected_with(with_body("i32.const 3 array.new_default $a ref.is_null drop"
                                           " ref.null func array.len drop"),
                                 "needs an array reference"));
        })
    })
    // A declared supertype has to be honest about the shapes.
    Category("a declared supertype must be one", {
        Test("adding fields to a struct is a refinement", {
            Expect(!validate("(module (type $a (sub (struct (field i32)))) (type (sub $a (struct (field i32) (field i64)))))"));
        })
        Test("changing a field type is not", {
            Expect(rejected_with("(module (type $a (sub (struct (field i32)))) (type (sub $a (struct (field i64)))))",
                                 "does not refine its supertype"));
        })
        Test("dropping a field is not", {
            Expect(rejected_with("(module (type $a (sub (struct (field i32) (field i64)))) (type (sub $a (struct (field i32)))))",
                                 "does not refine its supertype"));
        })
        Test("an immutable field may be refined", {
            Expect(!validate("(module (type $s (sub (struct))) (type $t (sub $s (struct)))"
                             " (type $a (sub (struct (field (ref $s))))) (type (sub $a (struct (field (ref $t))))))"));
        })
        Test("a mutable field is invariant", {
            Expect(rejected_with(
                "(module (type $s (sub (struct (field i32)))) (type $t (sub $s (struct (field i32) (field i64))))"
                " (type $a (sub (struct (field (mut (ref $s)))))) (type (sub $a (struct (field (mut (ref $t)))))))",
                "does not refine its supertype"));
        })
        Test("an array's element must match", {
            Expect(!validate("(module (type $a (sub (array i32))) (type (sub $a (array i32))))"));
            Expect(rejected_with("(module (type $a (sub (array i32))) (type (sub $a (array i64))))",
                                 "does not refine its supertype"));
        })
        Test("a function's parameters are contravariant and its results covariant", {
            const std::string types =
                "(module (type $s (sub (struct (field i32)))) (type $t (sub $s (struct (field i32) (field i64)))) ";
            Expect(!validate(types + "(type $f (sub (func (param (ref $t))))) (type (sub $f (func (param (ref $s))))))"));
            Expect(rejected_with(types + "(type $f (sub (func (param (ref $s))))) (type (sub $f (func (param (ref $t))))))",
                                 "does not refine its supertype"));
            Expect(!validate(types + "(type $f (sub (func (result (ref $s))))) (type (sub $f (func (result (ref $t))))))"));
            Expect(rejected_with(types + "(type $f (sub (func (result (ref $t))))) (type (sub $f (func (result (ref $s))))))",
                                 "does not refine its supertype"));
        })
    })
    // ref.null used to take only func and extern, which left a null struct or
    // array reference impossible to write.
    Category("ref.null over any heap type", {
        Test("a concrete type", {
            Expect(!validate(prelude + " (func (result (ref null $s)) ref.null $s))"));
        })
        Test("an abstract one", {
            Expect(!validate(prelude + " (func (result structref) ref.null struct))"));
            Expect(!validate(prelude + " (func (result arrayref) ref.null array))"));
            Expect(!validate(prelude + " (func (result anyref) ref.null any))"));
        })
        Test("a type index that does not exist", {
            Expect(rejected_with(prelude + " (func (result anyref) ref.null 99))", "type index not found for ref.null"));
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

// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The instructions that had no case in the function validator at all.
//
// They fell into its default no-op: they neither popped nor pushed, while the
// interpreter popped operands for them. A module could therefore pass
// validation and then empty the operand stack at run time, and Label::Values
// is a std::stack with no emptiness guard, so that is a segfault rather than
// a trap -- a guest bug becoming a host crash. Ten of the twelve did that
// (issue #21).
//
// Every "rejects" case below is a module that used to be accepted, and the
// crashing ones are marked. The last two, the convert instructions, never
// touched the operand stack, so they mis-typed rather than crashed; they are
// here too, which leaves nothing in the twelve without a case.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>

#include <list>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

std::optional<Exception::Exception> validate(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    return module_validate(module);
}

bool accepted(const std::string& wat) {
    std::optional<Exception::Exception> error = validate(wat);
    if (error) {
        std::cout << "      rejected: " << error->what() << std::endl;
        return false;
    }
    return true;
}

// Rejected, and for the stated reason rather than some later mismatch.
bool rejected_with(const std::string& wat, const std::string& fragment) {
    std::optional<Exception::Exception> error = validate(wat);
    if (!error) {
        std::cout << "      accepted, expected rejection mentioning '" << fragment << "'" << std::endl;
        return false;
    }
    if (std::string(error->what()).find(fragment) == std::string::npos) {
        std::cout << "      rejected for another reason: " << error->what() << std::endl;
        return false;
    }
    return true;
}

// Run an accepted module's exported "f" and return its results.
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

i32_t one_i32(const std::string& wat) {
    std::vector<Value> results = run(wat);
    return std::get<i32_t>(results.at(0));
}

const std::string with_struct = "(module (type $s (sub (struct (field $x i32)))) ";

} // namespace

Suite validate_instructions {
    Category("tail calls", {
        Test("a call with the right operands and results is accepted", {
            Expect(one_i32("(module (func $g (param i32) (result i32) local.get 0)"
                           " (func (export \"f\") (result i32) i32.const 5 return_call $g))") == 5);
        })
        Test("return_call with no operand is refused", {
            // Used to be accepted, and to crash the host.
            Expect(rejected_with("(module (func $g (param i32)) (func (export \"f\") return_call $g))",
                                 "empty validate value stack"));
        })
        Test("return_call whose results are not the caller's is refused", {
            Expect(rejected_with(
                "(module (func $g (param i32) (result i32) local.get 0)"
                " (func (export \"f\") (result i64) i32.const 5 return_call $g))",
                "return_call results do not match the enclosing function's"));
        })
        Test("a function index that does not exist", {
            Expect(rejected_with("(module (func (export \"f\") return_call 9))",
                                 "function index not found for return_call"));
        })
        Test("return_call_indirect works and needs its index", {
            Expect(one_i32(
                "(module (table 1 funcref) (type $t (func (param i32) (result i32)))"
                " (func $g (type $t) local.get 0) (elem (i32.const 0) func $g)"
                " (func (export \"f\") (result i32) i32.const 5 i32.const 0 return_call_indirect (type $t)))") == 5);
            Expect(rejected_with(
                "(module (table 1 funcref) (type $t (func (param i32)))"
                " (func (export \"f\") return_call_indirect (type $t)))",
                "empty validate value stack"));
        })
        Test("return_call_indirect needs a funcref table", {
            Expect(rejected_with(
                "(module (table 1 externref) (type $t (func))"
                " (func (export \"f\") i32.const 0 return_call_indirect (type $t)))",
                "table type should be funcref for return_call_indirect"));
        })
        Test("return_call_ref works and needs its operands", {
            Expect(one_i32(
                "(module (type $t (sub (func (param i32) (result i32))))"
                " (func $g (type $t) local.get 0)"
                " (func (export \"f\") (result i32) i32.const 5 ref.func $g return_call_ref $t))") == 5);
            Expect(rejected_with("(module (type $t (sub (func))) (func (export \"f\") return_call_ref $t))",
                                 "empty validate value stack"));
        })
    })
    Category("ref.eq", {
        Test("two references of the eq hierarchy", {
            Expect(one_i32("(module (func (export \"f\") (result i32)"
                           " i32.const 7 ref.i31 i32.const 7 ref.i31 ref.eq))") == 1);
            Expect(one_i32(with_struct + "(func (export \"f\") (result i32) (local $o (ref $s))"
                           " i32.const 1 struct.new $s local.set $o"
                           " local.get $o local.get $o ref.eq))") == 1);
        })
        Test("two numbers are refused", {
            // Used to be accepted, and to crash the host: ref.eq popped two
            // values the validator had not accounted for.
            Expect(rejected_with("(module (func (export \"f\") i32.const 1 i32.const 2 ref.eq drop drop))",
                                 "value type not match"));
        })
        Test("a funcref is not comparable", {
            Expect(rejected_with("(module (func (export \"f\") (result i32)"
                                 " ref.null func ref.null func ref.eq))",
                                 "reference to a different type"));
        })
    })
    Category("i31", {
        Test("ref.i31 then i31.get_s sign-extends, get_u does not", {
            Expect(one_i32("(module (func (export \"f\") (result i32) i32.const -1 ref.i31 i31.get_s))") == -1);
            Expect(one_i32("(module (func (export \"f\") (result i32) i32.const 7 ref.i31 i31.get_u))") == 7);
        })
        Test("ref.i31 needs an i32", {
            Expect(rejected_with("(module (func (export \"f\") ref.i31 drop))", "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") ref.null func ref.i31 drop))",
                                 "value type not match"));
        })
        Test("i31.get_s and i31.get_u need an i31 reference", {
            Expect(rejected_with("(module (func (export \"f\") i31.get_s drop))", "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") i31.get_u drop))", "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") (result i32) i32.const 1 i31.get_s))",
                                 "value type not match"));
        })
    })
    Category("ref.as_non_null", {
        Test("it takes the null off the type", {
            Expect(accepted(with_struct + "(func (param (ref null $s)) (result (ref $s))"
                            " local.get 0 ref.as_non_null))"));
        })
        Test("it needs a reference", {
            Expect(rejected_with("(module (func (export \"f\") ref.as_non_null drop))",
                                 "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") (result i32) i32.const 1 ref.as_non_null))",
                                 "ref.as_non_null should consume reference type"));
        })
    })
    Category("br_on_null and br_on_non_null", {
        Test("br_on_null falls through with the reference made non-null", {
            Expect(one_i32(with_struct +
                "(func (export \"f\") (result i32) block $h i32.const 9 struct.new $s"
                " br_on_null $h struct.get $s $x return end i32.const 0))") == 9);
        })
        Test("br_on_non_null sends the reference to its label", {
            Expect(one_i32(with_struct +
                "(func (export \"f\") (result i32) block $h (result (ref $s))"
                " i32.const 9 struct.new $s br_on_non_null $h i32.const 0 return end"
                " struct.get $s $x))") == 9);
        })
        Test("both need a reference", {
            Expect(rejected_with("(module (func (export \"f\") block br_on_null 0 end))",
                                 "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") block br_on_non_null 0 end))",
                                 "empty validate value stack"));
            Expect(rejected_with("(module (func (export \"f\") block i32.const 1 br_on_null 0 drop end))",
                                 "br_on_null should consume reference type"));
        })
        Test("a label index that does not exist", {
            Expect(rejected_with("(module (func (export \"f\") ref.null func br_on_null 9 drop))",
                                 "label index not found in br_on_null"));
        })
        Test("a label that cannot take what br_on_non_null sends it", {
            Expect(rejected_with(with_struct +
                "(func (export \"f\") (result i32) block $h (result i32)"
                " i32.const 9 struct.new $s br_on_non_null $h i32.const 0 return end))",
                "label of br_on_non_null does not accept the reference it is sent"));
        })
    })
    // The two that mis-typed rather than crashed. They move a reference
    // between the extern and any hierarchies and keep its nullability;
    // nothing happens to the value at run time, so only the type changes.
    Category("any.convert_extern and extern.convert_any", {
        Test("each takes a reference of the hierarchy it converts from", {
            Expect(accepted("(module (func (export \"f\") (result i32)"
                            " ref.null extern any.convert_extern ref.is_null))"));
            Expect(accepted("(module (func (export \"f\") (result i32)"
                            " ref.null any extern.convert_any ref.is_null))"));
        })
        Test("a number is refused", {
            // Both used to be accepted, giving a wrong result rather than a
            // diagnosis.
            Expect(rejected_with("(module (func (export \"f\") (result i32)"
                                 " i32.const 1 any.convert_extern))",
                                 "value type not match"));
            Expect(rejected_with("(module (func (export \"f\") (result i32)"
                                 " i32.const 1 extern.convert_any))",
                                 "value type not match"));
        })
        Test("the hierarchies are not interchangeable", {
            Expect(rejected_with("(module (func (export \"f\") (result i32)"
                                 " ref.null any any.convert_extern ref.is_null))",
                                 "reference to a different type"));
            Expect(rejected_with("(module (func (export \"f\") (result i32)"
                                 " ref.null extern extern.convert_any ref.is_null))",
                                 "reference to a different type"));
        })
        Test("the result is of the hierarchy converted to", {
            Expect(rejected_with("(module (func (export \"f\") (result externref)"
                                 " ref.null extern any.convert_extern))",
                                 "reference to a different type"));
        })
        Test("nullability is carried across", {
            Expect(accepted("(module (func (param (ref extern)) (result (ref any))"
                            " local.get 0 any.convert_extern))"));
            Expect(rejected_with("(module (func (param externref) (result (ref any))"
                                 " local.get 0 any.convert_extern))",
                                 "nullable reference where a non-null one is required"));
        })
    })
};

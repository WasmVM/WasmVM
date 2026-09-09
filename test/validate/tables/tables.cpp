// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// A table's and an elem segment's element type.
//
// It used to be a two-valued RefType, so a table of concrete references could
// not be declared at all -- and worse, the parser read exactly two keywords
// and silently took anything else for funcref, so (table 1 anyref) and even
// (table 1 i32) parsed and validated as a table of funcref. The module meant
// something other than what was written, with no diagnosis (issue #22).
//
// Now the element type is a value type that has to be a reference, and the
// instructions that move values in and out of a table are checked against the
// subtyping relation rather than equality.

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

WasmModule parse(const std::string& wat) {
    std::istringstream source(wat);
    return module_parse(source);
}

std::optional<Exception::Exception> validate(const std::string& wat) {
    WasmModule module = parse(wat);
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

bool rejected_with(const std::string& wat, const std::string& fragment) {
    std::optional<Exception::Exception> error;
    try {
        error = validate(wat);
    } catch (Exception::Parse& e) {
        if (std::string(e.what()).find(fragment) == std::string::npos) {
            std::cout << "      parse refused for another reason: " << e.what() << std::endl;
            return false;
        }
        return true;
    }
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

std::vector<Value> run(const std::string& wat) {
    WasmModule module = parse(wat);
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

// $t refines $s, so (ref $t) is below (ref $s) and not the other way round.
const std::string types =
    "(module (type $s (sub (struct (field $x i32))))"
    " (type $t (sub $s (struct (field $x i32) (field $y i64)))) ";

} // namespace

Suite validate_tables {
    Category("what an element type may be", {
        Test("the reference types that were already allowed", {
            Expect(accepted("(module (table 1 funcref))"));
            Expect(accepted("(module (table 1 externref))"));
        })
        Test("the abstract reference types that were silently funcref", {
            // Each of these parsed as a table of funcref and validated.
            Expect(accepted("(module (table 1 anyref))"));
            Expect(accepted("(module (table 1 structref))"));
            Expect(accepted("(module (table 1 arrayref))"));
            Expect(accepted("(module (table 1 eqref))"));
        })
        Test("a concrete reference", {
            Expect(accepted(types + "(table 1 (ref null $s)))"));
            Expect(accepted(types + "(elem (ref null $s)))"));
        })
        Test("a number is not a reference type", {
            // This used to parse as a table of funcref and validate.
            Expect(rejected_with("(module (table 1 i32))", "must be a reference type"));
            Expect(rejected_with("(module (elem i64))", "must be a reference type"));
        })
        Test("a non-nullable element type has nothing to start as", {
            // Every slot begins null and the type carries no initialiser.
            Expect(rejected_with(types + "(table 1 (ref $s)))", "should be nullable"));
        })
        Test("the element type survives the text and the binary", {
            const std::string source = types + "(table 1 (ref null $s)) (table 2 anyref)"
                                               " (elem (ref null $t) (item (ref.null $t))))";
            WasmModule before = parse(source);
            std::stringstream binary(std::ios::in | std::ios::out | std::ios::binary);
            module_encode(before, binary);
            WasmModule after = module_decode(binary);
            AssertEq(after.tables.size(), 2U);
            Expect(after.tables[0].elemtype == before.tables[0].elemtype);
            Expect(after.tables[1].elemtype == ValueType(ValueType::anyref));
            AssertEq(after.elems.size(), 1U);
            Expect(after.elems[0].type == before.elems[0].type);
        })
    })
    Category("moving values through a table", {
        Test("a reference goes in and comes back out", {
            std::vector<Value> results = run(types +
                "(table 1 (ref null $s))"
                " (func (export \"f\") (result i32)"
                "   i32.const 0 i32.const 7 struct.new $s table.set 0"
                "   i32.const 0 table.get 0 ref.cast (ref $s) struct.get $s $x))");
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 7);
        })
        Test("table.grow takes a reference of the element type", {
            // What the grow returns is left alone here: it answers with the
            // new size where the spec says the old one, which memory.grow
            // next door gets right. That is its own defect and nothing to do
            // with the element type, so this asserts the size and the fill.
            std::vector<Value> results = run(types +
                "(table 1 (ref null $s))"
                " (func (export \"f\") (result i32 i32)"
                "   ref.null $s i32.const 2 table.grow 0 drop"
                "   table.size 0 i32.const 2 table.get 0 ref.is_null))");
            AssertEq(results.size(), 2U);
            ExpectEq(std::get<i32_t>(results[0]), 3);
            ExpectEq(std::get<i32_t>(results[1]), 1); // grown slots start null
        })
        Test("table.fill takes a reference of the element type", {
            std::vector<Value> results = run(types +
                "(table 3 (ref null $s))"
                " (func (export \"f\") (result i32)"
                "   i32.const 0 ref.null $s i32.const 3 table.fill 0"
                "   i32.const 1 table.get 0 ref.is_null))");
            AssertEq(results.size(), 1U);
            ExpectEq(std::get<i32_t>(results[0]), 1);
        })
        Test("a subtype may be stored, a supertype may not", {
            Expect(accepted(types + "(table 1 (ref null $s))"
                            " (func i32.const 0 i32.const 1 i64.const 2 struct.new $t table.set 0))"));
            Expect(rejected_with(types + "(table 1 (ref null $t))"
                                 " (func i32.const 0 i32.const 1 struct.new $s table.set 0))",
                                 "reference to a different type"));
        })
        Test("a number may not be stored in a table of references", {
            Expect(rejected_with(types + "(table 1 (ref null $s))"
                                 " (func i32.const 0 i32.const 1 table.set 0))",
                                 "value type not match"));
        })
    })
    Category("the checks between a table, an elem segment and a call", {
        Test("table.copy needs the source to fit the destination", {
            Expect(accepted(types + "(table 1 (ref null $s)) (table 1 (ref null $t))"
                            " (func i32.const 0 i32.const 0 i32.const 0 table.copy 0 1))"));
            Expect(rejected_with(types + "(table 1 (ref null $t)) (table 1 (ref null $s))"
                                 " (func i32.const 0 i32.const 0 i32.const 0 table.copy 0 1))",
                                 "table types not match in table.copy"));
            Expect(rejected_with(types + "(table 1 (ref null $s)) (table 1 externref)"
                                 " (func i32.const 0 i32.const 0 i32.const 0 table.copy 0 1))",
                                 "table types not match in table.copy"));
        })
        Test("table.init needs the segment to fit the table", {
            Expect(accepted(types + "(table 1 (ref null $s)) (elem (ref null $t))"
                            " (func i32.const 0 i32.const 0 i32.const 0 table.init 0 0))"));
            Expect(rejected_with(types + "(table 1 (ref null $t)) (elem (ref null $s))"
                                 " (func i32.const 0 i32.const 0 i32.const 0 table.init 0 0))",
                                 "table & elem types not match in table.init"));
        })
        Test("call_indirect needs a table of the function hierarchy", {
            Expect(accepted("(module (type $f (func)) (table 1 funcref)"
                            " (func i32.const 0 call_indirect (type $f)))"));
            Expect(rejected_with(types + "(table 1 (ref null $s)) (type $f (func))"
                                 " (func i32.const 0 call_indirect (type $f)))",
                                 "table type should be funcref for call_indirect"));
            Expect(rejected_with("(module (type $f (func)) (table 1 anyref)"
                                 " (func i32.const 0 return_call_indirect (type $f)))",
                                 "table type should be funcref for return_call_indirect"));
        })
        Test("an elem item must fit the segment's type", {
            Expect(accepted(types + "(elem (ref null $s) (item (ref.null $t))))"));
            Expect(rejected_with(types + "(elem (ref null $t) (item (ref.null $s))))",
                                 "reference type mismatch"));
            Expect(rejected_with("(module (elem externref (item (ref.null func))))",
                                 "reference type mismatch"));
        })
    })
};

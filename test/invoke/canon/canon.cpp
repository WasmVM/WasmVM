// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Store-wide type identity (#39, and the foundation for #38 and import
// matching).
//
// A struct or array carries a local type index, which means nothing in another
// module. Two modules that both number a type 0 used to be judged the same
// type by that number alone, so a cast across a module boundary accepted an
// object of an unrelated shape. The registry gives every type a Store-wide
// CanonId instead: equal CanonId is identity, de-duplicated across modules, and
// a declared supertype is followed.
//
// Two kinds of test here. The first exercises `canonicalize_types` /
// `canon_matches` directly on parsed modules -- that is a unit test of the
// registry, needing no interpreter. The second runs a real two-module load
// into one Store and a `ref.test` across the boundary, which is the shape of
// the #39 bug; it needs a producer module exporting a maker and a consumer
// importing it, something the single-module `run` helper elsewhere cannot do,
// so the pair runner is new test infrastructure, local to this suite.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>
#include <instances/CanonRegistry.hpp>

#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

WasmModule parse_valid(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    return module;
}

// The CanonId of a module's local type `index`, after registering the module
// into `reg`.
CanonId id_of(const std::string& wat, index_t index, CanonRegistry& reg) {
    WasmModule module = parse_valid(wat);
    std::vector<CanonId> ids;
    canonicalize_types(ids, module, reg);
    return ids.at(index);
}

// Instantiate `producer` (which exports a no-argument "mk") and then `consumer`
// (which imports that one function and exports "f") into one shared Store, and
// run the consumer's "f". The func import is wired straight to the producer's
// export: instantiation does not type-check a function import, which is exactly
// the path the #39 object reaches the consumer by.
std::vector<Value> run_pair(const std::string& producer, const std::string& consumer) {
    static std::list<ModuleInst> instances; // stable addresses; FuncInst holds a reference
    static std::list<Store>      stores;
    Store& store = stores.emplace_back();

    WasmModule pmod = parse_valid(producer);
    ModuleInst& pinst = instances.emplace_back();
    module_instanciate(pinst, store, pmod, {});

    index_t mkaddr = index_npos;
    for (ExportInst& exported : pinst.exports) {
        if (exported.name == "mk" && exported.value.type == ExternVal::ExternType::Func) {
            mkaddr = exported.value.addr;
        }
    }
    if (mkaddr == index_npos) {
        throw Exception::Exception("producer module has no exported function 'mk'");
    }

    WasmModule cmod = parse_valid(consumer);
    ModuleInst& cinst = instances.emplace_back();
    std::vector<ExternVal> imports = { ExternVal{ ExternVal::ExternType::Func, mkaddr } };
    module_instanciate(cinst, store, cmod, imports);

    for (ExportInst& exported : cinst.exports) {
        if (exported.name == "f" && exported.value.type == ExternVal::ExternType::Func) {
            Stack stack(store);
            stack.invoke(exported.value.addr, {});
            return stack.run();
        }
    }
    throw Exception::Exception("consumer module has no exported function 'f'");
}

i32_t i32_at(const std::vector<Value>& values, size_t index) {
    return std::get<i32_t>(values.at(index));
}

// A producer whose exported "mk" returns a (ref (struct (field i32))).
const std::string producer_i32_struct =
    "(module"
    " (type $s (struct (field i32)))"
    " (func (export \"mk\") (result (ref $s)) i32.const 7 struct.new $s))";

// A consumer that imports "mk" and ref.tests its result against its own type 0.
std::string consumer_testing(const std::string& type0) {
    return "(module"
           " " + type0 +
           " (type $ft (func (result (ref 0))))"
           " (import \"a\" \"mk\" (func $mk (type $ft)))"
           " (func (export \"f\") (result i32) call $mk ref.test (ref 0)))";
}

} // namespace

Suite invoke_canon {
    Category("registry", {
        Test("isomorphic types in two modules share one CanonId", {
            CanonRegistry reg;
            CanonId a = id_of("(module (type $s (struct (field i32))))", 0, reg);
            CanonId b = id_of("(module (type $t (struct (field i32))))", 0, reg);
            ExpectEq(a, b);
        })
        Test("differently-shaped types get different CanonIds", {
            CanonRegistry reg;
            CanonId a = id_of("(module (type $s (struct (field i32))))", 0, reg);
            CanonId b = id_of("(module (type $t (struct (field i64) (field i64))))", 0, reg);
            Expect(a != b);
        })
        Test("field mutability is part of identity", {
            CanonRegistry reg;
            CanonId a = id_of("(module (type $s (struct (field i32))))", 0, reg);
            CanonId b = id_of("(module (type $t (struct (field (mut i32)))))", 0, reg);
            Expect(a != b);
        })
        Test("a recursive group rolls to one identity across modules", {
            // Type 0 has a field of reference-to-1 and vice versa: the two
            // indices are meaningless alone, but the rolled group is the same
            // in both modules. (Numeric indices, since the parser does not
            // resolve a forward type name inside a rec group.)
            const std::string rec =
                "(module (rec"
                " (type $a (struct (field (ref null 1))))"
                " (type $b (struct (field (ref null 0))))))";
            CanonRegistry reg;
            WasmModule m1 = parse_valid(rec);
            WasmModule m2 = parse_valid(rec);
            std::vector<CanonId> id1;
            std::vector<CanonId> id2;
            canonicalize_types(id1, m1, reg);
            canonicalize_types(id2, m2, reg);
            ExpectEq(id1.at(0), id2.at(0));
            ExpectEq(id1.at(1), id2.at(1));
            Expect(id1.at(0) != id1.at(1));
        })
        Test("canon_matches follows a declared supertype, one way", {
            CanonRegistry reg;
            WasmModule module = parse_valid(
                "(module"
                " (type $base (sub (struct (field i32))))"
                " (type $d (sub $base (struct (field i32) (field i64)))))");
            std::vector<CanonId> ids;
            canonicalize_types(ids, module, reg);
            Expect(canon_matches(ids.at(1), ids.at(0), reg));   // d <: base
            Expect(!canon_matches(ids.at(0), ids.at(1), reg));  // base is not <: d
        })
    })
    Category("cross-module cast", {
        Test("ref.test rejects an object of a same-numbered but different type (#39)", {
            // Consumer type 0 is { i64, i64 }; the imported object is a { i32 }.
            // The old code compared index 0 to index 0 and wrongly accepted it.
            std::vector<Value> r = run_pair(producer_i32_struct,
                consumer_testing("(type $s2 (struct (field i64) (field i64)))"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 0);
        })
        Test("ref.test still accepts an isomorphic type from another module", {
            // Consumer type 0 has the same shape as the producer's struct, so
            // the two roll to one CanonId and the cast must succeed.
            std::vector<Value> r = run_pair(producer_i32_struct,
                consumer_testing("(type $s2 (struct (field i32)))"));
            AssertEq(r.size(), 1U);
            ExpectEq(i32_at(r, 0), 1);
        })
    })
};

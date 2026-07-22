// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Integer division traps.
//
// These four opcodes have operand combinations that C++ `/` and `%` are simply
// not defined for. Handing them straight through is *undefined behavior*, not a
// wrong answer: on x86-64 `idiv` raises #DE, so the interpreter dies of SIGFPE
// and takes its embedder with it -- a guest bug becomes a host crash. The spec
// makes each of these a wasm trap, which an embedder can catch.
//
// So each case below asserts the trap, and the surrounding cases assert that
// ordinary division still computes what it should.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Stack.hpp>
#include <invoke/exception.hpp>

#include <cstdint>
#include <limits>
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

// A module whose "f" evaluates `lhs op rhs` and returns it.
std::string binop(const std::string& type, const std::string& op, const std::string& lhs,
                  const std::string& rhs) {
    return "(module (func (export \"f\") (result " + type + ") "
           "(" + type + "." + op + " (" + type + ".const " + lhs + ") (" + type + ".const " + rhs + "))))";
}

const std::string i32_min = std::to_string(std::numeric_limits<int32_t>::min());
const std::string i64_min = std::to_string(std::numeric_limits<int64_t>::min());

} // namespace

Suite numeric {
    Category("i32 division traps", {
        Test("div_s by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i32", "div_s", "1", "0")));
        })
        Test("div_u by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i32", "div_u", "1", "0")));
        })
        Test("rem_s by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i32", "rem_s", "1", "0")));
        })
        Test("rem_u by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i32", "rem_u", "1", "0")));
        })
        Test("div_s INT32_MIN / -1 traps", {
            // The one quotient outside the signed range.
            ExpectThrows(Exception::integer_overflow, run(binop("i32", "div_s", i32_min, "-1")));
        })
    })

    Category("i64 division traps", {
        Test("div_s by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i64", "div_s", "1", "0")));
        })
        Test("div_u by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i64", "div_u", "1", "0")));
        })
        Test("rem_s by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i64", "rem_s", "1", "0")));
        })
        Test("rem_u by zero traps", {
            ExpectThrows(Exception::integer_divide_by_zero, run(binop("i64", "rem_u", "1", "0")));
        })
        Test("div_s INT64_MIN / -1 traps", {
            ExpectThrows(Exception::integer_overflow, run(binop("i64", "div_s", i64_min, "-1")));
        })
    })

    Category("division still computes", {
        // The guards must not have changed any defined result. rem_s of the
        // overflow pair is the interesting one: it is 0 and must NOT trap,
        // even though the matching div_s does.
        Test("i32 rem_s INT32_MIN % -1 is 0 and does not trap", {
            std::vector<Value> r = run(binop("i32", "rem_s", i32_min, "-1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)0);
        })
        Test("i64 rem_s INT64_MIN % -1 is 0 and does not trap", {
            std::vector<Value> r = run(binop("i64", "rem_s", i64_min, "-1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)0);
        })
        Test("i32 div_s truncates toward zero", {
            std::vector<Value> r = run(binop("i32", "div_s", "-7", "2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-3);
        })
        Test("i32 rem_s takes the sign of the dividend", {
            std::vector<Value> r = run(binop("i32", "rem_s", "-7", "2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-1);
        })
        Test("i32 div_u treats operands as unsigned", {
            // -1 is 0xFFFFFFFF unsigned, so the quotient is large, not 0.
            std::vector<Value> r = run(binop("i32", "div_u", "-1", "2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0x7FFFFFFFu);
        })
        Test("i64 div_s INT64_MIN / 2 is representable", {
            std::vector<Value> r = run(binop("i64", "div_s", i64_min, "2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)(std::numeric_limits<int64_t>::min() / 2));
        })
    })
};

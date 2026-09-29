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

#include <cmath>
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

// A module whose "f" pushes `type.const value`, applies `ops`, and returns the
// `result` it leaves.
std::string convert(const std::string& result, const std::string& type, const std::string& value,
                    const std::string& ops) {
    return "(module (func (export \"f\") (result " + result + ") "
           + type + ".const " + value + " " + ops + "))";
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

    Category("float min/max propagate NaN", {
        // fmin / fmax: if either operand is a NaN, the result is a NaN. The
        // NaN branch used to pick the *other* operand, so min(nan, 1) was 1.
        Test("f32.min with NaN on the left", {
            std::vector<Value> r = run(binop("f32", "min", "nan", "1.0"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f32_t>(r[0])));
        })
        Test("f32.min with NaN on the right", {
            std::vector<Value> r = run(binop("f32", "min", "1.0", "nan"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f32_t>(r[0])));
        })
        Test("f32.max with NaN on the left", {
            std::vector<Value> r = run(binop("f32", "max", "nan", "1.0"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f32_t>(r[0])));
        })
        Test("f32.max with NaN on the right", {
            std::vector<Value> r = run(binop("f32", "max", "1.0", "nan"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f32_t>(r[0])));
        })
        Test("f64.min with NaN on the left", {
            std::vector<Value> r = run(binop("f64", "min", "nan", "1.0"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f64_t>(r[0])));
        })
        Test("f64.min with NaN on the right", {
            std::vector<Value> r = run(binop("f64", "min", "1.0", "nan"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f64_t>(r[0])));
        })
        Test("f64.max with NaN on the left", {
            std::vector<Value> r = run(binop("f64", "max", "nan", "1.0"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f64_t>(r[0])));
        })
        Test("f64.max with NaN on the right", {
            std::vector<Value> r = run(binop("f64", "max", "1.0", "nan"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::isnan(std::get<f64_t>(r[0])));
        })
        // Controls: the non-NaN branches are untouched.
        Test("f64.min of ordinary values", {
            std::vector<Value> r = run(binop("f64", "min", "2.0", "1.0"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<f64_t>(r[0]), (f64_t)1.0);
        })
        Test("f64.max of ordinary values", {
            std::vector<Value> r = run(binop("f64", "max", "2.0", "1.0"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<f64_t>(r[0]), (f64_t)2.0);
        })
        Test("f32.min of -0 and +0 is -0", {
            std::vector<Value> r = run(binop("f32", "min", "0.0", "-0.0"));
            AssertEq(r.size(), (size_t)1);
            Expect(std::signbit(std::get<f32_t>(r[0])));
        })
    })

    Category("shift counts are taken modulo the width", {
        // ishl / ishr_u / ishr_s: "Let k be i2 modulo N". A count of N is a
        // shift by 0, N+1 by 1, and -1 (all bits set) by N-1. Handing the raw
        // count to C++ `<<` / `>>` is undefined for counts >= N.
        Test("i32.shl by 32 is a shift by 0", {
            std::vector<Value> r = run(binop("i32", "shl", "1", "32"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)1);
        })
        Test("i32.shl by 33 is a shift by 1", {
            std::vector<Value> r = run(binop("i32", "shl", "1", "33"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)2);
        })
        Test("i32.shr_u by 32 is a shift by 0", {
            std::vector<Value> r = run(binop("i32", "shr_u", "-1", "32"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-1);
        })
        Test("i32.shr_s by 32 is a shift by 0", {
            std::vector<Value> r = run(binop("i32", "shr_s", "-8", "32"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-8);
        })
        Test("i32.shr_s by -1 is a shift by 31", {
            std::vector<Value> r = run(binop("i32", "shr_s", i32_min, "-1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-1);
        })
        Test("i64.shl by 64 is a shift by 0", {
            std::vector<Value> r = run(binop("i64", "shl", "1", "64"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)1);
        })
        Test("i64.shl by 65 is a shift by 1", {
            std::vector<Value> r = run(binop("i64", "shl", "1", "65"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)2);
        })
        Test("i64.shr_u by 64 is a shift by 0", {
            std::vector<Value> r = run(binop("i64", "shr_u", "-1", "64"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)-1);
        })
        Test("i64.shr_s by 64 is a shift by 0", {
            std::vector<Value> r = run(binop("i64", "shr_s", "-8", "64"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)-8);
        })
        Test("i64.shr_s by -1 is a shift by 63", {
            std::vector<Value> r = run(binop("i64", "shr_s", i64_min, "-1"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), (i64_t)-1);
        })
        // Controls: in-range counts are unchanged.
        Test("i32.shl by 4", {
            std::vector<Value> r = run(binop("i32", "shl", "3", "4"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)48);
        })
        Test("i32.shr_s keeps the sign", {
            std::vector<Value> r = run(binop("i32", "shr_s", "-16", "2"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)-4);
        })
        Test("i32.shr_u does not keep the sign", {
            std::vector<Value> r = run(binop("i32", "shr_u", "-16", "28"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)15);
        })
    })

    Category("saturating truncation", {
        // An operand that truncates into range gives that integer; only one
        // outside the range saturates. Converting a value above the signed
        // maximum straight to the signed type is undefined in C++, so the
        // unsigned forms returned the signed maximum on arm64 and the signed
        // minimum on x86-64 for anything at or above 2^31 (2^63).
        Test("i32.trunc_sat_f64_u of 2147483647", {
            std::vector<Value> r = run(convert("i32", "f64", "2147483647", "i32.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0x7FFFFFFFu);
        })
        Test("i32.trunc_sat_f64_u of 2147483648", {
            std::vector<Value> r = run(convert("i32", "f64", "2147483648", "i32.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0x80000000u);
        })
        Test("i32.trunc_sat_f64_u of 3000000000", {
            std::vector<Value> r = run(convert("i32", "f64", "3000000000", "i32.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xB2D05E00u);
        })
        Test("i32.trunc_sat_f64_u of 4294967040", {
            std::vector<Value> r = run(convert("i32", "f64", "4294967040", "i32.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xFFFFFF00u);
        })
        Test("i32.trunc_sat_f32_u of 2147483648", {
            std::vector<Value> r = run(convert("i32", "f64", "2147483648", "f32.demote_f64 i32.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0x80000000u);
        })
        Test("i32.trunc_sat_f32_u of 3000000000", {
            std::vector<Value> r = run(convert("i32", "f64", "3000000000", "f32.demote_f64 i32.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xB2D05E00u);
        })
        Test("i32.trunc_sat_f32_u of 4294967040", {
            std::vector<Value> r = run(convert("i32", "f64", "4294967040", "f32.demote_f64 i32.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xFFFFFF00u);
        })
        Test("i64.trunc_sat_f64_u of 2^63", {
            std::vector<Value> r = run(convert("i64", "f64", "9223372036854775808", "i64.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0x8000000000000000ull);
        })
        Test("i64.trunc_sat_f64_u of 1.2e19", {
            std::vector<Value> r = run(convert("i64", "f64", "1.2e19", "i64.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0xA688906BD8B00000ull);
        })
        Test("i64.trunc_sat_f32_u of 2^63", {
            std::vector<Value> r = run(convert("i64", "f64", "9223372036854775808", "f32.demote_f64 i64.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0x8000000000000000ull);
        })
        Test("i64.trunc_sat_f32_u of 1.2e19", {
            // 1.2e19 is not exact in f32; demoted, it is 0xA688900000000000.
            std::vector<Value> r = run(convert("i64", "f64", "1.2e19", "f32.demote_f64 i64.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0xA688900000000000ull);
        })
    })

    Category("saturating truncation of exactly 2^N", {
        // The saturation bound is the integer limit converted to the float
        // type, which rounds up to exactly 2^N. An operand equal to it must
        // saturate rather than fall through to the out-of-range conversion.
        // On arm64 the conversion happens to saturate by itself, so only an
        // x86-64 run tells the three signed cases apart from the unfixed code.
        Test("i32.trunc_sat_f32_u of 2^32 saturates", {
            std::vector<Value> r = run(convert("i32", "f32", "4294967296", "i32.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xFFFFFFFFu);
        })
        Test("i64.trunc_sat_f32_u of 2^64 saturates", {
            std::vector<Value> r = run(convert("i64", "f32", "18446744073709551616", "i64.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0xFFFFFFFFFFFFFFFFull);
        })
        Test("i64.trunc_sat_f64_u of 2^64 saturates", {
            std::vector<Value> r = run(convert("i64", "f64", "18446744073709551616", "i64.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0xFFFFFFFFFFFFFFFFull);
        })
        Test("i32.trunc_sat_f32_s of 2^31 saturates", {
            std::vector<Value> r = run(convert("i32", "f32", "2147483648", "i32.trunc_sat_f32_s"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), std::numeric_limits<i32_t>::max());
        })
        Test("i64.trunc_sat_f32_s of 2^63 saturates", {
            std::vector<Value> r = run(convert("i64", "f32", "9223372036854775808", "i64.trunc_sat_f32_s"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), std::numeric_limits<i64_t>::max());
        })
        Test("i64.trunc_sat_f64_s of 2^63 saturates", {
            std::vector<Value> r = run(convert("i64", "f64", "9223372036854775808", "i64.trunc_sat_f64_s"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i64_t>(r[0]), std::numeric_limits<i64_t>::max());
        })
        // Controls: the largest value below each bound still truncates.
        Test("i32.trunc_sat_f32_u of 4294967040 does not saturate", {
            std::vector<Value> r = run(convert("i32", "f32", "4294967040", "i32.trunc_sat_f32_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u32_t)std::get<i32_t>(r[0]), (u32_t)0xFFFFFF00u);
        })
        Test("i32.trunc_sat_f32_s of 2147483520 does not saturate", {
            std::vector<Value> r = run(convert("i32", "f32", "2147483520", "i32.trunc_sat_f32_s"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq(std::get<i32_t>(r[0]), (i32_t)2147483520);
        })
        Test("i64.trunc_sat_f64_s of 2^63 - 1024 does not saturate", {
            std::vector<Value> r = run(convert("i64", "f64", "9223372036854774784", "i64.trunc_sat_f64_s"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0x7FFFFFFFFFFFFC00ull);
        })
        Test("i64.trunc_sat_f64_u of 2^64 - 2048 does not saturate", {
            std::vector<Value> r = run(convert("i64", "f64", "18446744073709549568", "i64.trunc_sat_f64_u"));
            AssertEq(r.size(), (size_t)1);
            ExpectEq((u64_t)std::get<i64_t>(r[0]), (u64_t)0xFFFFFFFFFFFFF800ull);
        })
    })
};

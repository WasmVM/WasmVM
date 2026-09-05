// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Float literals in the text format.
//
// Two independent gaps used to reject spellings the spec allows (issue #11):
//
//   * The tokenizer only reached its inf/nan handling after a sign, so a bare
//     `nan`, `inf` or `nan:0x...` scanned as a keyword and never became a
//     literal at all.
//   * parse_f32 / parse_f64 accepted only a Float token, while the tokenizer
//     marks a literal with no '.', exponent or 'p' as an Integer. So
//     `f64.const 2` and `f64.const 0x10` were refused.
//
// Values matter as much as acceptance here: a literal that parses to the
// wrong number would be worse than one that is rejected, so every case below
// checks what it evaluated to.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <cmath>
#include <sstream>
#include <string>

using namespace WasmVM;
using namespace Testing;

namespace {

WasmModule parse(const std::string& wat) {
    std::istringstream in(wat);
    return module_parse(in);
}

// The literal is read back out of a global's initialiser.
f64_t f64_literal(const std::string& literal) {
    WasmModule m = parse("(module (global f64 (f64.const " + literal + ")))");
    return std::get<Instr::F64_const>(m.globals.at(0).init).value;
}

f32_t f32_literal(const std::string& literal) {
    WasmModule m = parse("(module (global f32 (f32.const " + literal + ")))");
    return std::get<Instr::F32_const>(m.globals.at(0).init).value;
}

bool rejected(const std::string& wat) {
    try {
        parse(wat);
        return false;
    } catch (Exception::Exception&) {
        return true;
    }
}

} // namespace

Suite literal {
    Category("inf and nan without a sign", {
        Test("f64 nan", {
            Expect(std::isnan(f64_literal("nan")));
        })
        Test("f64 inf", {
            Expect(std::isinf(f64_literal("inf")));
            Expect(f64_literal("inf") > 0);
        })
        Test("f32 nan", {
            Expect(std::isnan(f32_literal("nan")));
        })
        Test("f32 inf", {
            Expect(std::isinf(f32_literal("inf")));
        })
        Test("f64 nan:0x with a payload", {
            Expect(std::isnan(f64_literal("nan:0x4000000000000")));
        })
        Test("the signed spellings still work", {
            Expect(std::isnan(f64_literal("+nan")));
            Expect(std::isnan(f64_literal("-nan")));
            Expect(std::isinf(f64_literal("-inf")));
            Expect(f64_literal("-inf") < 0);
        })
    })
    Category("integer-shaped float literals", {
        Test("f64 decimal", {
            ExpectEq(f64_literal("2"), (f64_t)2.0);
            ExpectEq(f64_literal("+2"), (f64_t)2.0);
            ExpectEq(f64_literal("-2"), (f64_t)-2.0);
        })
        Test("f64 hex", {
            ExpectEq(f64_literal("0x10"), (f64_t)16.0);
        })
        Test("f32 decimal and hex", {
            ExpectEq(f32_literal("1"), (f32_t)1.0f);
            ExpectEq(f32_literal("-1"), (f32_t)-1.0f);
            ExpectEq(f32_literal("0x10"), (f32_t)16.0f);
        })
        Test("the forms that already worked are unchanged", {
            ExpectEq(f64_literal("1.0"), (f64_t)1.0);
            ExpectEq(f64_literal("1e3"), (f64_t)1000.0);
            ExpectEq(f64_literal("0x1.8p1"), (f64_t)3.0);
        })
    })
    Category("what must still be refused", {
        // The tokenizer routes a bare word to the number scanner only when it
        // is exactly inf/nan or nan:0x<hex>. A longer word that merely starts
        // with those letters stays a keyword, so these fail as unknown
        // instructions rather than as malformed numbers.
        Test("a keyword starting with nan is still a keyword", {
            Expect(rejected("(module (func nanxyz))"));
        })
        Test("a keyword starting with inf is still a keyword", {
            Expect(rejected("(module (func infinity))"));
        })
        Test("nan:0x with no hex digits is not a literal", {
            Expect(rejected("(module (global f64 (f64.const nan:0xzz)))"));
        })
        Test("an integer literal is still refused where an integer is wrong", {
            // i32.const takes an Integer token; a float literal there must
            // not start being accepted as a side effect.
            Expect(rejected("(module (global i32 (i32.const 1.5)))"));
        })
    })
};

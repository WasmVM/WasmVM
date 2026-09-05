// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Validation of the exception-handling instructions: try_table, throw and
// throw_ref, plus the tag section they depend on.
//
// Before try_table validation existed, the instruction fell into the
// validator's default no-op: no control frame was pushed while its `end`
// still popped one, so a perfectly valid module carrying try_table was
// rejected with "empty validate control stack" before it could ever run.
// The "accepts" cases below pin the fix; the "rejects" cases pin the new
// checks so they cannot silently degrade back into no-ops.
//
// Note on catch label indices: per spec they are resolved in the context
// ENCLOSING the try_table — label 0 is the innermost enclosing block, and
// the try_table's own label is not referable. Parser, validator and the
// runtime unwinder all follow this convention (issue #4).

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

// A module with one i32-parameter tag ($t0, tag index 0) and one void
// function whose body is `body`.
std::string with_tag(const std::string& body) {
    return "(module (type (func (param i32))) (tag $t0 (type 0)) (func " + body + "))";
}

} // namespace

Suite validate_exception {
    Category("try_table accepts", {
        Test("no catches", {
            Expect(!validate("(module (func try_table nop end))"));
        })
        Test("catch_all targeting the enclosing void label", {
            Expect(!validate("(module (func try_table (catch_all 0) nop end))"));
        })
        Test("catch targeting an enclosing label matching the tag params", {
            // Symbolic label: $h must resolve to index 0 (the block, not the
            // try_table itself).
            Expect(!validate(with_tag(
                "block $h (result i32) try_table (catch $t0 $h) nop end i32.const 7 end drop")));
        })
        Test("catch_ref targeting a label with tag params plus exnref", {
            Expect(!validate(with_tag(
                "block (result i32 exnref) try_table (catch_ref $t0 0) nop end unreachable end drop drop")));
        })
        Test("catch_all_ref targeting a label with exnref", {
            Expect(!validate(
                "(module (func block (result exnref) try_table (catch_all_ref 0) nop end unreachable end drop))"));
        })
        Test("frame balance: try_table around a loop", {
            // The exact shape that the missing control frame used to break.
            Expect(!validate("(module (func try_table (catch_all 0) loop nop end end))"));
        })
    })
    Category("try_table rejects", {
        Test("catch label index out of range", {
            Expect(validate("(module (func try_table (catch_all 5) nop end))").has_value());
        })
        Test("catch label index does not count the try_table itself", {
            // In a bare func the enclosing context has exactly one label
            // (the function's), so index 1 — which the pre-spec convention
            // would have accepted as the function label — is out of range.
            Expect(validate("(module (func try_table (catch_all 1) nop end))").has_value());
        })
        Test("catch cannot target the try_table's own label", {
            Expect(validate("(module (func try_table $l (catch_all $l) nop end))").has_value());
        })
        Test("catch tag index out of range", {
            Expect(validate("(module (func try_table (catch 3 0) nop end))").has_value());
        })
        Test("catch label type mismatch", {
            // Tag carries an i32 payload but the target label is void.
            Expect(validate(with_tag("try_table (catch $t0 0) nop end")).has_value());
        })
        Test("catch_all_ref label missing exnref", {
            Expect(validate("(module (func try_table (catch_all_ref 0) nop end))").has_value());
        })
    })
    Category("throw", {
        Test("accepts operands matching the tag params", {
            Expect(!validate(with_tag("i32.const 1 throw $t0")));
        })
        Test("rejects missing operands", {
            Expect(validate(with_tag("throw $t0")).has_value());
        })
        Test("rejects tag index out of range", {
            Expect(validate("(module (func throw 0))").has_value());
        })
    })
    Category("throw_ref", {
        Test("accepts an exnref operand", {
            Expect(!validate("(module (func (param exnref) local.get 0 throw_ref))"));
        })
        Test("rejects a non-exnref operand", {
            Expect(validate("(module (func i32.const 0 throw_ref))").has_value());
        })
    })
    Category("tag section", {
        Test("rejects a tag whose type index does not exist", {
            // This used to be spelled `(module (tag))`, which only had a
            // missing type index because an omitted typeuse fell back to
            // index 0 (issue #15). It now names the missing index outright,
            // which is what the case was always meant to check.
            Expect(validate("(module (tag (type 0)))").has_value());
        })
        Test("accepts a tag with no typeuse, which is the empty type", {
            Expect(!validate("(module (tag))").has_value());
        })
        Test("rejects a tag whose type has results", {
            Expect(validate("(module (type (func (result i32))) (tag (type 0)))").has_value());
        })
    })
};

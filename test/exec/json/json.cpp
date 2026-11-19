// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <sstream>
#include <string>

#include <json.hpp>
#include <exception.hpp>

using namespace Testing;

static Json::Value parse(const std::string& text){
    std::istringstream in(text);
    Json::Value value;
    in >> value;
    return value;
}

static std::string print(const Json::Value& value){
    std::ostringstream out;
    out << value;
    return out.str();
}

static bool parse_throws(const std::string& text){
    try {
        parse(text);
    } catch(WasmVM::Exception::Exception&) {
        return true;
    }
    return false;
}

Suite json {
    Category("number", {
        Test("negative_integer_parses", {
            Json::Value value = parse("-5");
            Assert(value.type() == Json::Value::Type::Integer);
            ExpectEq(value.get<Json::Value::Integer>(), (Json::Value::Integer)-5);
        })
        Test("negative_fraction_parses", {
            Json::Value value = parse("-2.5");
            Assert(value.type() == Json::Value::Type::Number);
            ExpectEq(value.get<Json::Value::Number>(), -2.5);
        })
        Test("integer_above_2_53_is_exact", {
            Json::Value value = parse("9007199254740993");
            Assert(value.type() == Json::Value::Type::Integer);
            ExpectEq(value.get<Json::Value::Integer>(), (Json::Value::Integer)9007199254740993LL);
            ExpectEq(print(value), std::string("9007199254740993"));
        })
        Test("int64_min_is_integer", {
            Json::Value value = parse("-9223372036854775808");
            Assert(value.type() == Json::Value::Type::Integer);
            ExpectEq(print(value), std::string("-9223372036854775808"));
        })
        Test("beyond_int64_falls_back_to_number", {
            Json::Value value = parse("9223372036854775808");
            Expect(value.type() == Json::Value::Type::Number);
        })
        Test("number_prints_all_significant_digits", {
            ExpectEq(print(parse("1234567.25")), std::string("1234567.25"));
            ExpectEq(print(parse("0.1")), std::string("0.1"));
        })
        Test("whole_number_stays_number_on_roundtrip", {
            std::string text = print(parse("3.0"));
            ExpectEq(text, std::string("3.0"));
            Expect(parse(text).type() == Json::Value::Type::Number);
        })
        Test("lone_minus_is_rejected", {
            Expect(parse_throws("-"));
        })
    })
    Category("string_output", {
        Test("quote_backslash_and_controls_are_escaped", {
            ExpectEq(print(parse("\"a\\\"b\\\\c\\nd\\te\\u0001\"")), std::string("\"a\\\"b\\\\c\\nd\\te\\u0001\""));
        })
        Test("escaped_output_parses_back", {
            Json::Value value = parse("\"x\\\"\\n\\\\y\"");
            ExpectEq(parse(print(value)).get<Json::Value::String>(), value.get<Json::Value::String>());
        })
        Test("object_key_is_escaped", {
            std::ostringstream out;
            out << Json::newline(false) << parse("{\"k\\\"ey\": 1}");
            ExpectEq(out.str(), std::string("{\"k\\\"ey\": 1}"));
        })
    })
    Category("unicode_escape", {
        Test("bmp_code_point_is_utf8", {
            ExpectEq(parse("\"\\u00e9\"").get<Json::Value::String>(), std::string("\xC3\xA9"));
        })
        Test("surrogate_pair_is_one_code_point", {
            ExpectEq(parse("\"\\ud83d\\ude00\"").get<Json::Value::String>(), std::string("\xF0\x9F\x98\x80"));
        })
        Test("lone_high_surrogate_is_rejected", {
            Expect(parse_throws("\"\\ud800\""));
        })
        Test("lone_low_surrogate_is_rejected", {
            Expect(parse_throws("\"\\udc00\""));
        })
        Test("non_hex_digit_is_rejected", {
            Expect(parse_throws("\"\\u00g0\""));
        })
    })
    Category("empty_container", {
        Test("empty_array_parses", {
            Json::Value value = parse("[]");
            Assert(value.type() == Json::Value::Type::Array);
            Expect(value.get<Json::Value::Array>().empty());
        })
        Test("empty_array_with_whitespace_parses", {
            Json::Value value = parse("[ \n]");
            Assert(value.type() == Json::Value::Type::Array);
            Expect(value.get<Json::Value::Array>().empty());
        })
        Test("empty_object_with_whitespace_parses", {
            Json::Value value = parse("{ \t}");
            Assert(value.type() == Json::Value::Type::Object);
            Expect(value.get<Json::Value::Object>().empty());
        })
        Test("nested_empty_array_parses", {
            Json::Value value = parse("{\"lines\":[],\"source\":{}}");
            Assert(value.type() == Json::Value::Type::Object);
            Json::Value::Object& object = value.get<Json::Value::Object>();
            Assert(object["lines"].type() == Json::Value::Type::Array);
            Expect(object["lines"].get<Json::Value::Array>().empty());
        })
        Test("unclosed_array_is_rejected", {
            Expect(parse_throws("[ "));
        })
    })
    Category("construct", {
        Test("scalars", {
            ExpectEq(print(Json::Value(Json::Value::Null())), std::string("null"));
            ExpectEq(print(Json::Value(true)), std::string("true"));
            ExpectEq(print(Json::Value(-7)), std::string("-7"));
            ExpectEq(print(Json::Value((uint32_t)4000000000u)), std::string("4000000000"));
            ExpectEq(print(Json::Value(2.5)), std::string("2.5"));
            ExpectEq(print(Json::Value("a\"b")), std::string("\"a\\\"b\""));
        })
        Test("integer_type", {
            Json::Value value((size_t)3);
            Expect(value.type() == Json::Value::Type::Integer);
        })
        Test("containers_round_trip", {
            Json::Value value(Json::Value::Object {
                {"seq", 1},
                {"body", Json::Value::Array {Json::Value("x"), Json::Value(false)}},
            });
            ExpectEq(print(parse(print(value))), print(value));
        })
    })
};

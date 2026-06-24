// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <string>

#include <CommandParser.hpp>

using namespace Testing;

// Option set that mixes a multi-character short alias ("-ns"), single-character
// flag/value aliases ("-v" / "-o"), and a positional. The buggy cluster-based
// parser treated "-ns" as the single-char flag "-n" (unknown) and aborted via
// std::exit, so these positive assertions guard against that regression.
//
// argv arrays live at file scope: brace initializers contain commas, which the
// preprocessor cannot carry through the Test(...) macro. The parser copies what
// it needs out of the options list during construction, so returning it by
// value (and letting the initializer_list backing die) is safe.
static const char* ARGV_SHORT_ALIAS[] = {"prog", "-ns", "main.wasm"};
static const char* ARGV_LONG_FORM[]   = {"prog", "--no-system", "main.wasm"};
static const char* ARGV_SINGLE_FLAG[]  = {"prog", "-v", "main.wasm"};
static const char* ARGV_VALUE_SHORT[]  = {"prog", "-o", "out.wasm", "in.wat"};
static const char* ARGV_VALUE_EQUALS[] = {"prog", "--output=out.wasm", "in.wat"};
static const char* ARGV_NONE[]         = {"prog", "main.wasm"};

static CommandParser parse(int argc, const char** argv){
    return CommandParser(argc, argv, {
        CommandParser::Optional("--no-system", "Disable system module path", "-ns"),
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--output", "Output file", 1, "-o"),
        CommandParser::Fixed("input", "input file", 1),
    });
}

Suite commandparser {
    Test("multi_char_short_alias", {
        CommandParser args = parse(3, ARGV_SHORT_ALIAS);
        Expect(args["no-system"].has_value());
        Expect(!args["version"].has_value());
        Assert(args["input"].has_value());
        ExpectEq(std::get<std::string>(args["input"].value()), std::string("main.wasm"));
    })
    Test("long_form_matches_alias", {
        CommandParser args = parse(3, ARGV_LONG_FORM);
        Expect(args["no-system"].has_value());
        Assert(args["input"].has_value());
        ExpectEq(std::get<std::string>(args["input"].value()), std::string("main.wasm"));
    })
    Test("single_char_flag", {
        CommandParser args = parse(3, ARGV_SINGLE_FLAG);
        Expect(args["version"].has_value());
        Expect(!args["no-system"].has_value());
    })
    Test("value_short_option_consumes_next_arg", {
        CommandParser args = parse(4, ARGV_VALUE_SHORT);
        Assert(args["output"].has_value());
        ExpectEq(std::get<std::string>(args["output"].value()), std::string("out.wasm"));
        Assert(args["input"].has_value());
        ExpectEq(std::get<std::string>(args["input"].value()), std::string("in.wat"));
    })
    Test("value_long_option_with_equals", {
        CommandParser args = parse(3, ARGV_VALUE_EQUALS);
        Assert(args["output"].has_value());
        ExpectEq(std::get<std::string>(args["output"].value()), std::string("out.wasm"));
    })
    Test("absent_option_is_nullopt", {
        CommandParser args = parse(2, ARGV_NONE);
        Expect(!args["no-system"].has_value());
        Expect(!args["output"].has_value());
        Assert(args["input"].has_value());
        ExpectEq(std::get<std::string>(args["input"].value()), std::string("main.wasm"));
    })
};

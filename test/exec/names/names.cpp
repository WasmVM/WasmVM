// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The custom "name" section (issue #51): filled from the text's ids, written
// by module_encode, read by module_decode (a malformed one dropped with a
// warning), copied into ModuleInst at instantiation, and remapped by the
// linker. The CLI chains (wasmvm-as, wasmvm-ld, readwasm) are tested by
// run_names.cmake.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <Linker.hpp>

#include <list>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

using NameMap = WasmNames::NameMap;
using IndirectNameMap = WasmNames::IndirectNameMap;

std::vector<std::string>& warnings(){
    static std::vector<std::string> list;
    static bool registered = false;
    if(!registered){
        Exception::Warning::regist([](std::string message){ list.emplace_back(message); });
        registered = true;
    }
    return list;
}

WasmModule parse(const std::string& wat){
    std::istringstream in(wat);
    WasmModule module = module_parse(in);
    if(std::optional<Exception::Exception> error = module_validate(module)){
        throw *error;
    }
    return module;
}

std::string encode(const WasmModule& module){
    std::ostringstream out;
    module_encode(module, out);
    return out.str();
}

WasmModule decode(const std::string& bytes){
    std::istringstream in(bytes);
    return module_decode(in);
}

std::string print(const NameMap& map){
    std::string out = "{";
    for(auto& [idx, name] : map){
        out += " " + std::to_string(idx) + ":" + name;
    }
    return out + " }";
}
std::string print(const IndirectNameMap& map){
    std::string out = "{";
    for(auto& [idx, inner] : map){
        out += " " + std::to_string(idx) + ":" + print(inner);
    }
    return out + " }";
}
template<typename T>
bool same(const T& actual, const T& expected, const char* what){
    if(actual == expected){
        return true;
    }
    std::cout << what << ": expected " << print(expected) << ", got " << print(actual) << std::endl;
    return false;
}

bool same_names(const WasmNames& a, const WasmNames& b){
    return a.module == b.module && a.funcs == b.funcs && a.locals == b.locals && a.labels == b.labels
        && a.types == b.types && a.tables == b.tables && a.mems == b.mems && a.globals == b.globals
        && a.elems == b.elems && a.datas == b.datas && a.fields == b.fields && a.tags == b.tags
        && a.type_params == b.type_params && a.tag_params == b.tag_params;
}

// Every subsection 0-13 non-empty, no imports, so it instantiates on its own.
const std::string every_subsection =
    "(module $mod\n"
    "  (type $ft (func (param $fp i32) (result i32)))\n"
    "  (type $st (struct (field $fa i32) (field $fb (mut i64))))\n"
    "  (table $tab 1 funcref)\n"
    "  (memory $mem 1)\n"
    "  (tag $tg (param $tgp i32))\n"
    "  (global $glob (mut i32) (i32.const 7))\n"
    "  (func $main (export \"main\") (param $arg i32) (result i32) (local $loc i32)\n"
    "    block $out\n"
    "      loop $in\n"
    "        local.get $arg\n"
    "        br_if $out\n"
    "        br $in\n"
    "      end\n"
    "    end\n"
    "    local.get $loc)\n"
    "  (elem $seg (table $tab) (i32.const 0) func $main)\n"
    "  (data $dat (memory $mem) (i32.const 0) \"x\")\n"
    ")\n";

// What every_subsection's ids become.
WasmNames every_subsection_names(){
    WasmNames names;
    names.module = "mod";
    names.funcs = {{0, "main"}};
    names.locals = {{0, {{0, "arg"}, {1, "loc"}}}};
    names.labels = {{0, {{0, "out"}, {1, "in"}}}};
    names.types = {{0, "ft"}, {1, "st"}};
    names.tables = {{0, "tab"}};
    names.mems = {{0, "mem"}};
    names.globals = {{0, "glob"}};
    names.elems = {{0, "seg"}};
    names.datas = {{0, "dat"}};
    names.fields = {{1, {{0, "fa"}, {1, "fb"}}}};
    names.tags = {{0, "tg"}};
    names.type_params = {{0, {{0, "fp"}}}};
    names.tag_params = {{0, {{0, "tgp"}}}};
    return names;
}

// A module of only a magic/version header and the given sections.
std::string with_sections(const std::string& sections){
    return std::string("\0asm\x01\0\0\0", 8) + sections;
}

std::string leb(u32_t value){
    std::string out;
    do{
        u8_t byte = value & 0x7f;
        value >>= 7;
        if(value){
            byte |= 0x80;
        }
        out += (char)byte;
    }while(value);
    return out;
}

std::string str(const std::string& s){
    return leb(s.size()) + s;
}

std::string custom(const std::string& name, const std::string& payload){
    std::string body = str(name) + payload;
    return std::string(1, '\0') + leb(body.size()) + body;
}

std::string subsection(u8_t id, const std::string& content){
    return std::string(1, (char)id) + leb(content.size()) + content;
}

// A name map of (index, name) pairs, written as given.
std::string namemap(const std::vector<std::pair<u32_t, std::string>>& entries){
    std::string out = leb(entries.size());
    for(auto& [idx, name] : entries){
        out += leb(idx) + str(name);
    }
    return out;
}

const std::string module_name = subsection(0, str("m"));
const std::string func_names = subsection(1, namemap({{0, "f"}}));

// Decodes a module whose only section is a name section with `payload`, and
// returns the warnings it raised.
std::vector<std::string> decode_warnings(const std::string& bytes, WasmModule& module){
    warnings().clear();
    module = decode(bytes);
    return warnings();
}

WasmModule link(const std::vector<std::pair<std::string, std::string>>& inputs, Linker::Config::StartMode start){
    Linker::Config config;
    config.start_func = start;
    Linker linker(config);
    for(const auto& [path, wat] : inputs){
        std::string binary = encode(parse(wat));
        std::istringstream stream(binary);
        linker.consume(path, stream);
    }
    return linker.get();
}

} // namespace

Suite names_suite {
    Category("text", {
        Test("every subsection from the text's ids", {
            WasmModule module = parse(every_subsection);
            WasmNames expected = every_subsection_names();
            const WasmNames& n = module.names;
            Expect(n.module == expected.module);
            Expect(same(n.funcs, expected.funcs, "funcs"));
            Expect(same(n.locals, expected.locals, "locals"));
            Expect(same(n.labels, expected.labels, "labels"));
            Expect(same(n.types, expected.types, "types"));
            Expect(same(n.tables, expected.tables, "tables"));
            Expect(same(n.mems, expected.mems, "mems"));
            Expect(same(n.globals, expected.globals, "globals"));
            Expect(same(n.elems, expected.elems, "elems"));
            Expect(same(n.datas, expected.datas, "datas"));
            Expect(same(n.fields, expected.fields, "fields"));
            Expect(same(n.tags, expected.tags, "tags"));
            Expect(same(n.type_params, expected.type_params, "type_params"));
            Expect(same(n.tag_params, expected.tag_params, "tag_params"));
        })
        Test("names follow imports moved ahead of definitions", {
            WasmModule module = parse(
                "(module (func $def) (import \"m\" \"f\" (func $imp))"
                " (global $gdef i32 (i32.const 0)) (import \"m\" \"g\" (global $gimp i32)))");
            Expect(same(module.names.funcs, NameMap{{0, "imp"}, {1, "def"}}, "funcs"));
            Expect(same(module.names.globals, NameMap{{0, "gimp"}, {1, "gdef"}}, "globals"));
        })
        Test("label indices are function-wide and in source order, unlabelled blocks included", {
            WasmModule module = parse(
                "(module (func block end block $b block $c end end loop $l end)"
                " (func block $x end))");
            Expect(same(module.names.labels, IndirectNameMap{
                {0, {{1, "b"}, {2, "c"}, {3, "l"}}},
                {1, {{0, "x"}}}
            }, "labels"));
        })
        Test("folded-together function types keep the earliest name", {
            WasmModule module = parse(
                "(module (type $first (func (param $a i32))) (type $second (func (param $b i32))))");
            Expect(module.types.size() == 1);
            Expect(same(module.names.types, NameMap{{0, "first"}}, "types"));
            Expect(same(module.names.type_params, IndirectNameMap{{0, {{0, "a"}}}}, "type_params"));
        })
        Test("a module without ids has no names", {
            WasmModule module = parse("(module (func (param i32) (local i32)) (global i32 (i32.const 0)))");
            Expect(module.names.empty());
            Expect(encode(module).find("name") == std::string::npos);
        })
    })
    Category("binary", {
        Test("every subsection survives encode and decode", {
            WasmModule decoded = decode(encode(parse(every_subsection)));
            Expect(same_names(decoded.names, every_subsection_names()));
        })
        Test("decode -> instantiate fills ModuleInst::names", {
            WasmModule decoded = decode(encode(parse(every_subsection)));
            Store store;
            ModuleInst instance;
            module_instanciate(instance, store, decoded, {});
            Expect(same_names(instance.names, every_subsection_names()));
            Expect(!instance.names.empty());
        })
        Test("an index past its index space is kept", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", subsection(1, namemap({{7, "far"}})))), module);
            Expect(w.empty());
            Expect(same(module.names.funcs, NameMap{{7, "far"}}, "funcs"));
        })
        Test("an unknown subsection is skipped silently", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name + subsection(14, "junk") + subsection(15, ""))), module);
            Expect(w.empty());
            Expect(module.names.module == std::optional<std::string>("m"));
        })
        Test("custom sections anywhere, several in a row", {
            // Two custom sections before the type section and one between
            // sections: before #51 the second one stopped the decoder from
            // reading the type section at all.
            std::string type_section = std::string("\x01\x04\x01\x60\x00\x00", 6);
            std::string func_section = std::string("\x03\x02\x01\x00", 4);
            std::string code_section = std::string("\x0a\x04\x01\x02\x00\x0b", 6);
            WasmModule module = decode(with_sections(
                custom("a", "x") + custom("b", "") + type_section
                + custom("c", "y") + func_section + code_section
                + custom("name", func_names) + custom("d", "")));
            Expect(module.types.size() == 1);
            Expect(module.funcs.size() == 1);
            Expect(same(module.names.funcs, NameMap{{0, "f"}}, "funcs"));
        })
    })
    Category("malformed", {
        Test("repeated subsection", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name + func_names + func_names)), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("out-of-order subsection", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", func_names + module_name)), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("subsection length past the section", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name + std::string("\x01\x7f", 2) + namemap({{0, "f"}}))), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("subsection longer than its content", {
            WasmModule module;
            std::string content = namemap({{0, "f"}}) + "?";
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name + subsection(1, content))), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("section length past the end of the module", {
            WasmModule module;
            // The module ends right after a whole subsection, but the section
            // length claims more bytes than are there.
            std::string body = str("name") + module_name;
            std::string section = std::string(1, '\0') + leb(body.size() + 5) + body;
            std::vector<std::string> w = decode_warnings(with_sections(section), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("invalid UTF-8", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name + subsection(1, namemap({{0, std::string("\xc0\xaf", 2)}})))), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("indices not increasing in a name map", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", subsection(1, namemap({{1, "a"}, {1, "b"}})))), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("indices not increasing in an indirect name map", {
            WasmModule module;
            std::string inner = namemap({{0, "x"}});
            std::string content = leb(2) + leb(3) + inner + leb(2) + inner;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", subsection(2, content))), module);
            Expect(w.size() == 1);
            Expect(module.names.empty());
        })
        Test("a second name section is ignored with a warning", {
            WasmModule module;
            std::vector<std::string> w = decode_warnings(
                with_sections(custom("name", module_name) + custom("name", func_names)), module);
            Expect(w.size() == 1);
            Expect(module.names.module == std::optional<std::string>("m"));
            Expect(module.names.funcs.empty());
        })
        Test("the module still loads and validates", {
            WasmModule module;
            std::string type_section = std::string("\x01\x04\x01\x60\x00\x00", 6);
            std::vector<std::string> w = decode_warnings(
                with_sections(type_section + custom("name", func_names + module_name)), module);
            Expect(w.size() == 1);
            Expect(module.types.size() == 1);
            Expect(!module_validate(module));
        })
    })
    Category("linker", {
        Test("names follow the index remap", {
            const std::string b =
                "(module $b"
                "  (type $tb (func (param $pb i32)))"
                "  (import \"a\" \"f\" (func $f_from_b (type $tb)))"
                "  (import \"env\" \"ext\" (func $ext_b (type $tb)))"
                "  (global $g i32 (i32.const 2))"
                "  (func $start_b)"
                "  (start $start_b))";
            const std::string a =
                "(module $a"
                "  (type $ta (func (param $pa i32)))"
                "  (import \"env\" \"ext\" (func $ext_a (type $ta)))"
                "  (global $g i32 (i32.const 1))"
                "  (func $f (export \"f\") (type $ta) (local $la i32) block $blk end)"
                "  (func $start_a)"
                "  (start $start_a))";
            WasmModule out = link({{"b", b}, {"a", a}}, Linker::Config::StartMode::All);
            const WasmNames& n = out.names;
            // Output funcs: import env.ext (0), start_b (1), f (2), start_a (3),
            // the composed start function (4).
            Assert(out.imports.size() == 1);
            Assert(out.funcs.size() == 4);
            Expect(!n.module);
            Expect(same(n.funcs, NameMap{{0, "ext_b"}, {1, "start_b"}, {2, "f"}, {3, "start_a"}}, "funcs"));
            Expect(same(n.locals, IndirectNameMap{{0, {{0, "pb"}}}, {2, {{0, "pa"}, {1, "la"}}}}, "locals"));
            Expect(same(n.labels, IndirectNameMap{{2, {{0, "blk"}}}}, "labels"));
            Expect(same(n.globals, NameMap{{0, "g"}, {1, "g"}}, "globals"));
            // (func (param i32)) is shared: b is linked first.
            Expect(same(n.types, NameMap{{0, "tb"}}, "types"));
            Expect(same(n.type_params, IndirectNameMap{{0, {{0, "pb"}}}}, "type_params"));
        })
    })
};

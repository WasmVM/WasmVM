#ifndef WASMVM_PP_STRUCTURE_NAMES
#define WASMVM_PP_STRUCTURE_NAMES

#include <map>
#include <string>
#include <optional>
#include <Types.hpp>

namespace WasmVM {

// The contents of the custom "name" section: the spec's subsections and the
// extended-name-section proposal's. Maps are ordered, so they iterate in the
// increasing index order the binary format requires.
struct WasmNames {
    using NameMap = std::map<index_t, std::string>;
    using IndirectNameMap = std::map<index_t, NameMap>;

    std::optional<std::string> module;  // 0
    NameMap funcs;                      // 1
    IndirectNameMap locals;             // 2  funcidx -> localidx
    IndirectNameMap labels;             // 3  funcidx -> function-wide, source-order label index
    NameMap types;                      // 4
    NameMap tables;                     // 5
    NameMap mems;                       // 6
    NameMap globals;                    // 7
    NameMap elems;                      // 8
    NameMap datas;                      // 9
    IndirectNameMap fields;             // 10 typeidx -> fieldidx
    NameMap tags;                       // 11
    IndirectNameMap type_params;        // 12 typeidx (function types) -> paramidx
    IndirectNameMap tag_params;         // 13 tagidx -> paramidx

    bool empty() const {
        return !module && funcs.empty() && locals.empty() && labels.empty() && types.empty()
            && tables.empty() && mems.empty() && globals.empty() && elems.empty() && datas.empty()
            && fields.empty() && tags.empty() && type_params.empty() && tag_params.empty();
    }
};

}

#endif

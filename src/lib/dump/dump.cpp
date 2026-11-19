/**
 * Copyright 2023 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "dump.hpp"
#include <Util.hpp>

#include <algorithm>
#include <string>
#include <ostream>
#include <utility>
#include <vector>

using namespace WasmVM;

// Names of definitions come through DumpNames::current(); with none set the
// sections print as before.
static const std::string* def_name(DumpSpace space, index_t idx){
    DumpNames* names = DumpNames::current();
    return names ? names->define(space, idx) : nullptr;
}

static index_t import_count(const WasmModule& module, size_t alternative){
    index_t count = 0;
    for(const WasmImport& import : module.imports){
        if(import.desc.index() == alternative){
            count += 1;
        }
    }
    return count;
}

std::ostream& WasmVM::dump_types(std::ostream& stream, const WasmModule& module){
    // Types outside every explicit (rec ...) group print bare; a group
    // prints as one (rec ...) with its members indented.
    std::vector<RecGroup> recs(module.recs.begin(), module.recs.end());
    std::sort(recs.begin(), recs.end(), [](const RecGroup& a, const RecGroup& b){ return a.first < b.first; });
    size_t k = 0;
    for(index_t i = 0; i < module.types.size();){
        if(k < recs.size() && recs[k].first == i){
            stream << "  (rec" << std::endl;
            for(index_t j = 0; j < recs[k].count && (i + j) < module.types.size(); ++j){
                dump_subtype(stream, module.types[i + j], "    ", i + j);
            }
            stream << "  )" << std::endl;
            i += recs[k].count;
            k += 1;
        }else{
            dump_subtype(stream, module.types[i], "  ", i);
            i += 1;
        }
    }
    return stream;
}

std::ostream& WasmVM::dump_imports(std::ostream& stream, const WasmModule& module){
    index_t funcs = 0, tables = 0, mems = 0, globals = 0, tags = 0;
    for(const WasmImport& import : module.imports){
        const std::string* name = std::visit(overloaded {
            [&](index_t){ index_t i = funcs++; return def_name(DumpSpace::func, i); },
            [&](const TableType&){ index_t i = tables++; return def_name(DumpSpace::table, i); },
            [&](const MemType&){ index_t i = mems++; return def_name(DumpSpace::mem, i); },
            [&](const GlobalType&){ index_t i = globals++; return def_name(DumpSpace::global, i); },
            [&](const TagType&){ index_t i = tags++; return def_name(DumpSpace::tag, i); },
        }, import.desc);
        dump_import(stream, import, name);
    }
    return stream;
}

std::ostream& WasmVM::dump_funcs(std::ostream& stream, const WasmModule& module){
    index_t base = import_count(module, 0);
    for(index_t i = 0; i < module.funcs.size(); ++i){
        dump_func(stream, module.funcs[i], base + i);
    }
    return stream;
}

std::ostream& WasmVM::dump_tables(std::ostream& stream, const WasmModule& module){
    index_t base = import_count(module, 1);
    for(index_t i = 0; i < module.tables.size(); ++i){
        dump_table(stream, module.tables[i], def_name(DumpSpace::table, base + i));
    }
    return stream;
}

std::ostream& WasmVM::dump_mems(std::ostream& stream, const WasmModule& module){
    index_t base = import_count(module, 2);
    for(index_t i = 0; i < module.mems.size(); ++i){
        dump_mem(stream, module.mems[i], def_name(DumpSpace::mem, base + i));
    }
    return stream;
}

std::ostream& WasmVM::dump_tags(std::ostream& stream, const WasmModule& module){
    // Imported tags were printed with the imports, so the module-defined
    // ones keep their place in the index space.
    index_t base = import_count(module, 4);
    for(index_t i = 0; i < module.tags.size(); ++i){
        stream << "  (tag";
        dump_def_name(stream, def_name(DumpSpace::tag, base + i));
        stream << " (type ";
        dump_ref(stream, DumpSpace::type, module.tags[i]) << ") )" << std::endl;
    }
    return stream;
}

std::ostream& WasmVM::dump_globals(std::ostream& stream, const WasmModule& module){
    index_t base = import_count(module, 3);
    for(index_t i = 0; i < module.globals.size(); ++i){
        dump_global(stream, module.globals[i], def_name(DumpSpace::global, base + i));
    }
    return stream;
}

std::ostream& WasmVM::dump_exports(std::ostream& stream, const WasmModule& module){
    for(const WasmExport& export_ : module.exports){
        stream << export_;
    }
    return stream;
}

std::ostream& WasmVM::dump_start(std::ostream& stream, const WasmModule& module){
    if(module.start){
        stream << "  (start ";
        dump_ref(stream, DumpSpace::func, module.start.value()) << ")" << std::endl;
    }
    return stream;
}

std::ostream& WasmVM::dump_elems(std::ostream& stream, const WasmModule& module){
    for(index_t i = 0; i < module.elems.size(); ++i){
        dump_elem(stream, module.elems[i], def_name(DumpSpace::elem, i));
    }
    return stream;
}

std::ostream& WasmVM::dump_datas(std::ostream& stream, const WasmModule& module){
    for(index_t i = 0; i < module.datas.size(); ++i){
        dump_data(stream, module.datas[i], def_name(DumpSpace::data, i));
    }
    return stream;
}

std::ostream& WasmVM::module_dump(const WasmModule& module, std::ostream& stream){
    DumpNames names(module);
    DumpNames::Scope scope(names);
    stream << "(module";
    dump_def_name(stream, names.module());
    stream << " " << std::endl;
    dump_types(stream, module);
    dump_imports(stream, module);
    dump_funcs(stream, module);
    dump_tables(stream, module);
    dump_mems(stream, module);
    // Tags sit between the memories and the globals, matching the binary
    // section order.
    dump_tags(stream, module);
    dump_globals(stream, module);
    dump_exports(stream, module);
    dump_start(stream, module);
    dump_elems(stream, module);
    dump_datas(stream, module);
    stream << ")" << std::endl;
    return stream;
}

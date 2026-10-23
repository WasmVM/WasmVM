// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <variant>
#include <algorithm>
#include <map>

#include <WasmVM.hpp>
#include <exception.hpp>
#include <instances/Store.hpp>
#include <host.hpp>

#include "CommandParser.hpp"
#include "ModuleQueue.hpp"
#include "color.hpp"

using namespace WasmVM;

static std::vector<ExternVal> match_imports(Store& store, std::map<std::filesystem::path, ModuleInst>& moduleinsts, ModuleQueue::Node& node){
    std::vector<ExternVal> externvals;
    // The importing module is not instantiated yet, so give its declared import
    // types Store-wide identities now. module_instanciate canonicalizes the same
    // module again; de-duplication makes that idempotent -- the same CanonIds.
    std::vector<CanonId> import_type_ids;
    canonicalize_types(import_type_ids, node.module, store.canon);
    // A function export satisfies a function import when its type is a Store-wide
    // subtype of the import's: structural refinement, and declared supertypes too
    // (canon_matches). This replaces the raw index equality FuncType== did, which
    // confused same-numbered types across modules. The CLI's module functions and
    // its sysenv host functions all carry a canonical type (the latter stamped at
    // registration), so there is no structural fallback here. An embedder that
    // registers host functions another way does its own import matching, not this.
    auto func_import_matches = [&](index_t export_addr, index_t import_typeidx) -> bool {
        return canon_matches(store.funcs[export_addr].canon, import_type_ids[import_typeidx], store.canon);
    };
    for(size_t idx = 0; idx < node.import_paths.size(); ++idx){
        std::filesystem::path import_path = node.import_paths[idx];
        if(!moduleinsts.contains(import_path)){
            throw Exception::Exception(std::string("module '") + import_path.filename().string() + "' imported before instanciate");
        }
        ModuleInst& moduleinst = moduleinsts[import_path];
        WasmImport& import = node.module.imports[idx];
        // For function imports, match by name AND type so that host modules can
        // register same-name variants with different signatures (e.g. i32 vs i64
        // pointer params for wasm32/wasm64 callers).
        auto export_it = std::find_if(moduleinst.exports.begin(), moduleinst.exports.end(),
            [&](ExportInst& expo) {
                if(expo.name != import.name) return false;
                if(expo.value.type != ExternVal::ExternType::Func) return true;
                if(!std::holds_alternative<index_t>(import.desc)) return true;
                return func_import_matches(expo.value.addr, std::get<index_t>(import.desc));
            });
        if(export_it == moduleinst.exports.end()){
            throw Exception::Exception(std::string("import '") + import.name + "' not found in module '" + import_path.filename().string() + "'");
        }
        switch(export_it->value.type){
            case ExternVal::ExternType::Func :
                if(!std::holds_alternative<index_t>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                if(!func_import_matches(export_it->value.addr, std::get<index_t>(import.desc))){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match function type");
                }
            break;
            case ExternVal::ExternType::Table : {
                if(!std::holds_alternative<TableType>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                TableType imported = std::get<TableType>(import.desc);
                TableType exported = store.tables[export_it->value.addr].type;
                if(exported.limits.min < imported.limits.min){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match table limit");
                }
                if(imported.limits.max.has_value()){
                    if(!exported.limits.max.has_value() || (exported.limits.max.value() > imported.limits.max.value())){
                        throw Exception::Exception(std::string("import '") + import.name + "' not match table limit");
                    }
                }
                if(imported.elemtype != exported.elemtype){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match reference type");
                }
            }break;
            case ExternVal::ExternType::Mem : {
                if(!std::holds_alternative<MemType>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                MemType imported = std::get<MemType>(import.desc);
                MemType exported = store.mems[export_it->value.addr].type;
                if(exported.min < imported.min){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match memory limit");
                }
                if(imported.max.has_value()){
                    if(!exported.max.has_value() || (exported.max.value() > imported.max.value())){
                        throw Exception::Exception(std::string("import '") + import.name + "' not match memory limit");
                    }
                }
            }break;
            case ExternVal::ExternType::Global :
                if(!std::holds_alternative<GlobalType>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                if(store.globals[export_it->value.addr].type != std::get<GlobalType>(import.desc).type){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match function type");
                }
            break;
        }
        externvals.emplace_back(export_it->value);
    }
    return externvals;
}

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--no-system", "Disable importing modules from system module path", "-ns"),
        CommandParser::Optional("--no-parent", "Disable importing modules from module parent path", "-np"),
        CommandParser::Optional("--force", "Skip validation", "-f"),
        CommandParser::Fixed("main_module", "main WebAssembly module binary"),
        CommandParser::Fixed("args", "Arguments passed to main module", (unsigned int)index_npos),
        CommandParser::Optional("--extra", "Path to find modules", (unsigned int)index_npos, "-e")
    },
        "wasmvm : WasmVM WebAssembly virtual machine"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM version " WASMVM_VERSION << std::endl;
        return 0;
    }

    try {
        // Specify paths
        std::vector<std::filesystem::path> extra_paths;
        bool check_parent = true;
        std::optional<std::filesystem::path> system_path;
        bool run_validate = true;
        if(args["extra"]){
            std::vector<std::string> paths = std::get<std::vector<std::string>>(args["extra"].value());
            for(std::string path : paths){
                extra_paths.emplace_back(path);
            }
        }
        if(!args["main_module"]){
            throw Exception::Exception("no main module");
        }
        if(args["no-parent"]){
            check_parent = false;
        }
        if(!args["no-system"]){
            system_path = DEFAULT_MODULE_PATH;
        }
        if(args["force"]){
            run_validate = false;
        }
        // Runtime structures
        Store store;
        std::map<std::filesystem::path, ModuleInst> moduleinsts;

        // Get module queue
        std::string main_module = std::get<std::string>(args["main_module"].value());

        // Host modules
        #ifdef HOST_MODULES
        wasmvm_args.clear();
        wasmvm_args.push_back(main_module);
        if (args["args"]) {
            auto extra_args = std::get<std::vector<std::string>>(args["args"].value());
            for (auto& a : extra_args)
                wasmvm_args.push_back(std::move(a));
        }
        host_modules_instanciate(moduleinsts, store);
        #endif
        ModuleQueue module_queue(main_module, moduleinsts, extra_paths, system_path, check_parent, run_validate);

        // Instanciate
        while(!module_queue.empty()){
            ModuleQueue::Node node = module_queue.pop();
            std::vector<ExternVal> externvals = match_imports(store, moduleinsts, node);
            try {
                moduleinsts.emplace(node.file_path, ModuleInst());
                module_instanciate(moduleinsts[node.file_path], store, node.module, externvals);
            }catch(Exception::Exception &e){
                throw Exception::Exception(node.file_path.filename().string() + ": " + e.what());
            }
        }
        
    }catch(Exception::Exception &e){
        std::cerr << std::filesystem::path(argv[0]).filename().string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

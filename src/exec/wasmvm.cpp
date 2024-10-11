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

static std::vector<ExternVal> match_imports(const Store& store, std::map<std::filesystem::path, ModuleInst>& moduleinsts, ModuleQueue::Node& node){
    std::vector<ExternVal> externvals;
    for(size_t idx = 0; idx < node.import_paths.size(); ++idx){
        std::filesystem::path import_path = node.import_paths[idx];
        if(!moduleinsts.contains(import_path)){
            throw Exception::Exception(std::string("module '") + import_path.filename().string() + "' imported before instanciate");
        }
        ModuleInst& moduleinst = moduleinsts[import_path];
        WasmImport& import = node.module.imports[idx];
        auto export_it = std::find_if(moduleinst.exports.begin(), moduleinst.exports.end(), [import](ExportInst& expo) {
            return expo.name == import.name;
        });
        if(export_it == moduleinst.exports.end()){
            throw Exception::Exception(std::string("import '") + import.name + "' not found in module '" + import_path.filename().string() + "'");
        }
        switch(export_it->value.type){
            case ExternVal::ExternType::Func :
                if(!std::holds_alternative<index_t>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                if(store.funcs[export_it->value.addr].type != node.module.types[std::get<index_t>(import.desc)]){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match function type");
                }
            break;
            case ExternVal::ExternType::Table : {
                if(!std::holds_alternative<TableType>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                TableType imported = std::get<TableType>(import.desc);
                TableType exported = store.tables[export_it->value.addr].type;
                if(imported.limits.min < exported.limits.min){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match table limit");
                }
                if(exported.limits.max.has_value()){
                    if(!imported.limits.max.has_value() || (imported.limits.max.value() < exported.limits.max.value())){
                        throw Exception::Exception(std::string("import '") + import.name + "' not match table limit");
                    }
                }
                if(imported.reftype != exported.reftype){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match reference type");
                }
            }break;
            case ExternVal::ExternType::Mem : {
                if(!std::holds_alternative<MemType>(import.desc)){
                    throw Exception::Exception(std::string("import '") + import.name + "' type not match");
                }
                MemType imported = std::get<MemType>(import.desc);
                MemType exported = store.mems[export_it->value.addr].type;
                if(imported.min < exported.min){
                    throw Exception::Exception(std::string("import '") + import.name + "' not match memory limit");
                }
                if(exported.max.has_value()){
                    if(!imported.max.has_value() || (imported.max.value() < exported.max.value())){
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
        CommandParser::Fixed("extra_path", "Path to find modules", (unsigned int)index_npos)
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
        if(args["extra_path"]){
            std::vector<std::string> paths = std::get<std::vector<std::string>>(args["extra_path"].value());
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
        if(!args["no-sysyem"]){
            system_path = DEFAULT_MODULE_PATH;
        }
        if(args["force"]){
            run_validate = false;
        }
        // Runtime structures
        Store store;
        std::map<std::filesystem::path, ModuleInst> moduleinsts;

        // Host modules
        #ifdef HOST_MODULES
        host_modules_instanciate(moduleinsts, store);
        #endif

        // Get module queue
        std::string main_module = std::get<std::string>(args["main_module"].value());
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

// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include <WasmVM.hpp>
#include <exception.hpp>

#include "CommandParser.hpp"
#include "color.hpp"
#include "../lib/dump/dump.hpp"

using namespace WasmVM;

int main(int argc, char const *argv[]){  
    // Parse argv
    CommandParser args(argc, argv, {
        CommandParser::Optional("--version", "Show version", "-v"),
        CommandParser::Optional("--force", "Skip validation", "-f"),
        CommandParser::Optional("--all", "Show all sections (default)", "-a"),
        CommandParser::Optional("--type", "Show types (ignored by --all)", "-t"),
        CommandParser::Optional("--import", "Show imports (ignored by --all)", "-i"),
        CommandParser::Optional("--func", "Show functions (ignored by --all)", "-c"),
        CommandParser::Optional("--table", "Show tables (ignored by --all)", "-b"),
        CommandParser::Optional("--memory", "Show memories (ignored by --all)", "-m"),
        CommandParser::Optional("--global", "Show globals (ignored by --all)", "-g"),
        CommandParser::Optional("--export", "Show exports (ignored by --all)", "-x"),
        CommandParser::Optional("--start", "Show start (ignored by --all)", "-s"),
        CommandParser::Optional("--elem", "Show elements (ignored by --all)", "-e"),
        CommandParser::Optional("--data", "Show datas (ignored by --all)", "-d"),
        CommandParser::Fixed("input_file", "Input file in WebAssembly binary format"),
    },
        "readwasm : WasmVM wasm read/disassembler"
    );
    // Warnings
    Exception::Warning::regist([](std::string message){
        std::cerr << COLOR_Warning ": " << message << std::endl;
    });
    // version
    if(args["version"]){
        std::cerr << "WasmVM assembler version " VERSION << std::endl;
        return 0;
    }
    // input file
    if(!args["input_file"]){
        std::cerr << "readwasm : " COLOR_Error ": no input file" << std::endl;
        return -1;
    }
    // Input path
    std::filesystem::path input_path(std::get<std::string>(args["input_file"].value()));
    if(!std::filesystem::exists(input_path)){
        std::cerr << "readwasm : " COLOR_Error ": no such file '" << input_path.string() << "'" << std::endl;
        return -1;
    }

    try {
        // Decode
        std::ifstream input_file(input_path);
        WasmModule decoded_module = module_decode(input_file);
        input_file.close();

        // Validate
        if(!args["force"]){
            auto validate_result = module_validate(decoded_module);
            if(validate_result){
                throw validate_result.value();
            }
        }

        // Dump
        if(args["all"] || (!args["type"] && !args["import"] && !args["func"] && !args["table"] && !args["memory"] &&
            !args["global"] && !args["export"] && !args["start"] && !args["elem"] && !args["data"])
        ){
            module_dump(decoded_module, std::cout);
            return 0;
        }
        // Type
        if(args["type"]){
            for(const FuncType& type : decoded_module.types){
                std::cout << type;
            }
        }
        // Import
        if(args["import"]){
            for(const WasmImport& import : decoded_module.imports){
                std::cout << import;
            }
        }
        // Func
        if(args["func"]){
            for(const WasmFunc& func : decoded_module.funcs){
                std::cout << func;
            }
        }
        // Table
        if(args["table"]){
            for(const TableType& table : decoded_module.tables){
                std::cout << table;
            }
        }
        // Memory
        if(args["memory"]){
            for(const MemType& mem : decoded_module.mems){
                std::cout << mem;
            }
        }
        // Global
        if(args["global"]){
            for(const WasmGlobal& global : decoded_module.globals){
                std::cout << global;
            }
        }
        // Export
        if(args["export"]){
            for(const WasmExport& ex : decoded_module.exports){
                std::cout << ex;
            }
        }
        // Start
        if(args["export"]){
            if(decoded_module.start){
                std::cout << "  (start " << decoded_module.start.value() << ")" << std::endl;
            }
        }
        // Elem
        if(args["elem"]){
            for(const WasmElem& elem : decoded_module.elems){
                std::cout << elem;
            }
        }
        // Data
        if(args["data"]){
            for(const WasmData& data : decoded_module.datas){
                std::cout << data;
            }
        }

    }catch(Exception::Decode &e){
        std::ios::fmtflags flags = std::cerr.flags();
        std::cerr << input_path.string() << ":" << std::hex << std::showbase << e.location << " " COLOR_Error ": " << e.what() << std::endl;
        std::cerr.setf(flags);
        return -1;
    }catch(Exception::Exception &e){
        std::cerr << input_path.string() << ": " COLOR_Error ": " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

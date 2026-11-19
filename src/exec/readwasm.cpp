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
        std::cerr << "WasmVM assembler version " WASMVM_VERSION << std::endl;
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
        // Decode (binary: a wasm module is a byte stream; text mode would
        // mangle 0x0d/0x1a bytes on Windows)
        std::ifstream input_file(input_path, std::ios::binary | std::ios::in);
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
        // Selected sections, named like the full dump
        DumpNames names(decoded_module);
        DumpNames::Scope scope(names);
        // Type
        if(args["type"]){
            // Each type on its own line, rec groups not shown (as before)
            for(index_t i = 0; i < decoded_module.types.size(); ++i){
                dump_subtype(std::cout, decoded_module.types[i], "  ", i);
            }
        }
        // Import
        if(args["import"]){
            dump_imports(std::cout, decoded_module);
        }
        // Func
        if(args["func"]){
            dump_funcs(std::cout, decoded_module);
        }
        // Table
        if(args["table"]){
            dump_tables(std::cout, decoded_module);
        }
        // Memory
        if(args["memory"]){
            dump_mems(std::cout, decoded_module);
        }
        // Global
        if(args["global"]){
            dump_globals(std::cout, decoded_module);
        }
        // Export
        if(args["export"]){
            dump_exports(std::cout, decoded_module);
        }
        // Start
        if(args["start"]){
            dump_start(std::cout, decoded_module);
        }
        // Elem
        if(args["elem"]){
            dump_elems(std::cout, decoded_module);
        }
        // Data
        if(args["data"]){
            dump_datas(std::cout, decoded_module);
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

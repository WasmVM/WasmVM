// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "CommandParser.hpp"

#include <queue>
#include <Util.hpp>
#include <iostream>
#include <cstdlib>

#include "color.hpp"

static void help(std::filesystem::path program, std::string desc,
    std::vector<std::variant<CommandParser::Fixed, CommandParser::Optional>> options
){
    std::cerr << desc << std::endl;
    // Usage
    std::cerr << std::endl << "Usage: " << program.filename().string() << " [-h | --help]";
    bool showed_dashdash = false;
    for(std::variant<CommandParser::Fixed, CommandParser::Optional> option : options){
        std::visit(overloaded {
            [&](CommandParser::Fixed& opt){
                if(!showed_dashdash){
                    std::cerr << " [--]";
                    showed_dashdash = true;
                }
                std::cerr << " " << opt.name;
                switch(opt.number){
                    case (unsigned)-1:
                        std::cerr << "...";
                    break;
                    case 0:
                    case 1:
                    break;
                    default:
                        std::cerr << "{" << opt.number << "}";
                    break;
                }
            },
            [&](CommandParser::Optional& opt){
                std::cerr << " [";
                if(!opt.alias.empty()){
                    std::cerr << opt.alias << " | ";
                }
                std::cerr << opt.name << "]";
                switch(opt.number){
                    case (unsigned)-1:
                        std::cerr << "...";
                    break;
                    case 0:
                    break;
                    case 1:
                        std::cerr << "?";
                    break;
                    default:
                        std::cerr << "{" << opt.number << "}";
                    break;
                }
            }
        }, option);
    }

    // Options
    std::cerr << std::endl << std::endl << "Options:" << std::endl;
    std::cerr << "  --help, -h\t\tShow help message" << std::endl;
    std::cerr << "  --\t\t\tEnd of options; remaining arguments are positional" << std::endl;
    for(auto& option : options){
        std::visit(overloaded {
            [&](CommandParser::Fixed& opt){
                std::cerr << "  " << opt.name << "\t\t" << opt.desc << std::endl;
            },
            [&](CommandParser::Optional& opt){
                std::cerr << "  " << opt.name;
                if(!opt.alias.empty()){
                    std::cerr << ", " << opt.alias;
                }
                std::cerr << "\t\t" << opt.desc << std::endl;
            }
        }, option);
    }

    std::cerr << std::endl;
}

CommandParser::CommandParser(int argc, const char* argv[],
    std::initializer_list<std::variant<Fixed, Optional>> options, std::string desc
) : program(argv[0]){

    std::queue<Fixed> fixed;
    std::unordered_map<std::string, Optional> optmap;
    std::unordered_map<std::string, std::string> aliases;
    for(auto option : options){
        std::visit(overloaded {
            [&](Fixed& opt){
                Fixed& f = fixed.emplace(opt);
                if(f.number == 0){
                    f.number = 1;
                }
            },
            [&](Optional& opt){
                optmap[opt.name] = opt;
                if(!opt.alias.empty()){
                    aliases[opt.alias] = opt.name;
                }
            }
        }, option);
    }

    // Consume values for a resolved option name. argi points to the first
    // candidate value on entry and is advanced past everything consumed.
    auto process_option = [&](const std::string& name, std::optional<std::string> inline_val, int& argi){
        Arg& value = args[name];
        switch(optmap[name].number){
            case 0:
                value.emplace<std::monostate>();
            break;
            case 1:
                if(inline_val){
                    value.emplace<std::string>(inline_val.value());
                } else {
                    if(argi >= argc){
                        std::cerr << COLOR_Error ": option '" << name << "' requires an argument" << std::endl;
                        std::exit(-1);
                    }
                    value.emplace<std::string>(argv[argi++]);
                }
            break;
            default:{
                std::vector<std::string>& values = value.emplace<std::vector<std::string>>();
                if(inline_val){
                    values.emplace_back(inline_val.value());
                }
                for(unsigned remain = optmap[name].number; remain > 0 && argi < argc; --remain, ++argi){
                    std::string argstr(argv[argi]);
                    if(argstr == "--" || optmap.contains(argstr) || aliases.contains(argstr)){
                        break;
                    }
                    if(argstr == "--help" || argstr == "-h"){
                        help(program, desc, options);
                        std::exit(0);
                    }
                    values.emplace_back(argstr);
                }
            }break;
        }
    };

    int argi = 1;
    bool options_done = false;

    while(argi < argc){
        std::string arg(argv[argi]);

        if(arg == "--help" || arg == "-h"){
            help(program, desc, options);
            std::exit(0);
        }

        // End-of-options marker
        if(!options_done && arg == "--"){
            options_done = true;
            ++argi;
            continue;
        }

        // Option parsing (disabled after --)
        if(!options_done && arg.size() >= 2 && arg[0] == '-'){
            if(arg[1] == '-'){
                // Long option: --opt or --opt=value
                std::string opt_name;
                std::optional<std::string> inline_val;
                auto eq = arg.find('=');
                if(eq != std::string::npos){
                    opt_name = arg.substr(0, eq);
                    inline_val = arg.substr(eq + 1);
                } else {
                    opt_name = arg;
                }
                if(aliases.contains(opt_name)){
                    opt_name = aliases[opt_name];
                }
                if(!optmap.contains(opt_name)){
                    std::cerr << COLOR_Error ": unknown option '" << opt_name << "'" << std::endl;
                    help(program, desc, options);
                    std::exit(-1);
                }
                ++argi;
                process_option(opt_name, inline_val, argi);
            } else {
                // Short option cluster: -abc, -oVALUE, -o=VALUE
                // Process characters left to right. A flag (number==0) is consumed
                // and the next character starts a new option. A value-taking option
                // eats the rest of the string (with optional leading '=') or the
                // next argv.
                ++argi;
                for(size_t i = 1; i < arg.size(); ){
                    std::string short_opt = std::string("-") + arg[i];
                    std::string resolved = aliases.contains(short_opt) ? aliases[short_opt]
                                         : optmap.contains(short_opt) ? short_opt : "";
                    if(resolved.empty()){
                        std::cerr << COLOR_Error ": unknown option '" << short_opt << "'" << std::endl;
                        help(program, desc, options);
                        std::exit(-1);
                    }
                    ++i;
                    if(optmap[resolved].number == 0){
                        args[resolved].emplace<std::monostate>();
                    } else {
                        // Rest of the cluster string is the inline value (strip leading '=')
                        std::string rest = arg.substr(i);
                        if(!rest.empty() && rest[0] == '='){
                            rest = rest.substr(1);
                        }
                        auto inline_val = rest.empty() ? std::optional<std::string>{}
                                                       : std::make_optional(rest);
                        process_option(resolved, inline_val, argi);
                        break; // rest of cluster was consumed as value
                    }
                }
            }
            continue;
        }

        // Positional argument
        if(!fixed.empty()){
            Fixed& option = fixed.front();
            Arg& value = args[option.name];
            bool do_pop = true;
            if(option.number == 1){
                value.emplace<std::string>(arg);
                ++argi;
            } else {
                std::vector<std::string>& values = std::holds_alternative<std::vector<std::string>>(value)
                    ? std::get<std::vector<std::string>>(value)
                    : value.emplace<std::vector<std::string>>();
                for(unsigned remain = option.number; remain > 0 && argi < argc; --remain, ++argi){
                    std::string argstr(argv[argi]);
                    if(argstr == "--help" || argstr == "-h"){
                        help(program, desc, options);
                        std::exit(0);
                    }
                    if(!options_done){
                        if(argstr == "--"){
                            options_done = true;
                            ++argi;
                            do_pop = false;
                            break;
                        }
                        if(optmap.contains(argstr) || aliases.contains(argstr)){
                            do_pop = false;
                            break;
                        }
                    }
                    values.emplace_back(argstr);
                }
            }
            if(do_pop){
                fixed.pop();
            }
        } else {
            std::cerr << COLOR_Error ": too many arguments" << std::endl;
            help(program, desc, options);
            std::exit(-1);
        }
    }
}

std::optional<CommandParser::Arg> CommandParser::operator[](const std::string key){
    if(args.contains(key)){
        return args[key];
    }else if(args.contains(std::string("--") + key)){
        return args[std::string("--") + key];
    }
    return std::nullopt;
}

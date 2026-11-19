// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Profiler (#61). Built only under WASMVM_PROFILE (src/lib/CMakeLists.txt).

#include <Profiler.hpp>
#include <instances/Stack.hpp>
#include "../dump/dump.hpp"

#include <chrono>
#include <sstream>
#include <variant>

using namespace WasmVM;

Profiler::Profiler(clock_t now) : clock(now), sink(slot_count, 0){
    current_instrs = sink.data();
}

void Profiler::set_module_name(const ModuleInst& module, std::string name){
    module_names[&module] = std::move(name);
}

int64_t Profiler::raw_now() const {
    if(clock != nullptr){
        return clock();
    }
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

// Time with every debugger stop taken out.
int64_t Profiler::now() const {
    return raw_now() - paused_total;
}

void Profiler::pause(){
    if(pause_depth++ == 0){
        pause_start = raw_now();
    }
}

void Profiler::resume(){
    if(pause_depth > 0 && --pause_depth == 0){
        paused_total += raw_now() - pause_start;
    }
}

void Profiler::update_current(){
    current_instrs = sink.data();
    if(!running.empty()){
        Section& section = *running.back();
        if(!section.frames.empty() && !section.frames.back().stats->host){
            current_instrs = section.frames.back().stats->instrs.data();
        }
    }
}

void Profiler::on_enter(index_t funcaddr){
    // A frame pushed outside run() enters when run() begins (begin_stack).
    if(running.empty()){
        return;
    }
    Section& section = *running.back();
    FuncStats& stats = section.funcs[funcaddr];
    if(stats.calls == 0){
        stats.host = !std::holds_alternative<WasmFunc>(section.store->funcs[funcaddr].body);
        if(!stats.host){
            stats.instrs.assign(slot_count, 0);
        }
    }
    stats.calls += 1;
    stats.active += 1;
    Node* parent = section.frames.empty() ? &section.root : section.frames.back().node;
    std::unique_ptr<Node>& node = parent->children[funcaddr];
    if(!node){
        node = std::make_unique<Node>();
        node->funcaddr = funcaddr;
    }
    if(!section.has_entry){
        section.has_entry = true;
        section.entry_funcaddr = funcaddr;
    }
    section.frames.push_back(Entry {&stats, node.get(), now(), 0});
    update_current();
}

void Profiler::on_exit(){
    if(running.empty() || running.back()->frames.empty()){
        return;
    }
    Section& section = *running.back();
    Entry entry = section.frames.back();
    section.frames.pop_back();
    const int64_t duration = now() - entry.start;
    const int64_t exclusive = duration - entry.children;
    entry.stats->exclusive += exclusive;
    entry.stats->active -= 1;
    if(entry.stats->active == 0){
        entry.stats->inclusive += duration;
    }
    entry.node->exclusive += exclusive;
    if(!section.frames.empty()){
        section.frames.back().children += duration;
    }
    update_current();
}

void Profiler::begin_stack(const Stack& stack){
    Section& section = *sections.emplace_back(std::make_unique<Section>());
    section.store = &stack.store;
    running.push_back(&section);
    for(const Frame& frame : stack.frames){
        on_enter(frame.funcaddr);
    }
    update_current();
}

void Profiler::end_stack() noexcept {
    if(running.empty()){
        return;
    }
    try {
        while(!running.back()->frames.empty()){
            on_exit();
        }
        resolve(*running.back());
    } catch(...) {
        // Out of memory while closing the report: keep what was recorded.
    }
    running.pop_back();
    update_current();
}

void Profiler::finish(){
    while(!running.empty()){
        end_stack();
    }
}

Profiler::Name Profiler::name_of(const Store& store, index_t funcaddr) const {
    const FuncInst& func = store.funcs[funcaddr];
    const ModuleInst& module = func.module;
    Name name;
    if(auto found = module_names.find(&module); found != module_names.end()){
        name.module = found->second;
    }else if(module.names.module){
        name.module = *module.names.module;
    }
    std::optional<index_t> index;
    for(index_t i = 0; i < module.funcaddrs.size(); ++i){
        if(module.funcaddrs[i] == funcaddr){
            index = i;
            break;
        }
    }
    if(index){
        if(auto named = module.names.funcs.find(*index); named != module.names.funcs.end()){
            name.func = "$" + named->second;
            return name;
        }
    }
    for(const ExportInst& exported : module.exports){
        if(exported.value.type == ExternVal::ExternType::Func && exported.value.addr == funcaddr){
            name.func = exported.name;
            return name;
        }
    }
    name.func = index ? ("func " + std::to_string(*index)) : ("funcaddr " + std::to_string(funcaddr));
    return name;
}

static std::string frame_of(const std::string& module, const std::string& func){
    return module.empty() ? func : (module + ":" + func);
}

// Names, entry and mnemonics are taken while the Store is still alive: the
// report may be written after it is gone.
void Profiler::resolve(Section& section) const {
    const Store& store = *section.store;
    for(auto& [funcaddr, stats] : section.funcs){
        section.names[funcaddr] = name_of(store, funcaddr);
        if(stats.host){
            continue;
        }
        const WasmFunc& func = std::get<WasmFunc>(store.funcs[funcaddr].body);
        for(size_t s = 0; s < slot_count; ++s){
            if(stats.instrs[s] == 0 || section.mnemonics.contains(s)){
                continue;
            }
            for(const WasmInstr& instr : func.body){
                if(slot(instr.opcode) == s){
                    std::ostringstream text;
                    text << instr;
                    std::string mnemonic = text.str();
                    mnemonic = mnemonic.substr(0, mnemonic.find_first_of(" \n\t"));
                    section.mnemonics[s] = mnemonic;
                    break;
                }
            }
        }
    }
    if(section.has_entry){
        const Name& name = section.names[section.entry_funcaddr];
        section.entry = frame_of(name.module, name.func);
    }
}

Json::Value Profiler::report() const {
    Json::Value::Array stacks;
    for(size_t index = 0; index < sections.size(); ++index){
        const Section& section = *sections[index];
        Json::Value::Array functions;
        for(const auto& [funcaddr, stats] : section.funcs){
            Json::Value::Object function;
            function["funcaddr"] = Json::Value(funcaddr);
            auto named = section.names.find(funcaddr);
            function["module"] = Json::Value((named != section.names.end()) ? named->second.module : std::string());
            function["name"] = Json::Value((named != section.names.end()) ? named->second.func : std::string());
            function["host"] = Json::Value(stats.host);
            function["calls"] = Json::Value(stats.calls);
            function["inclusive_ns"] = Json::Value(stats.inclusive);
            function["exclusive_ns"] = Json::Value(stats.exclusive);
            if(!stats.host){
                uint64_t total = 0;
                Json::Value::Object instructions;
                for(size_t s = 0; s < slot_count; ++s){
                    if(stats.instrs[s] == 0){
                        continue;
                    }
                    total += stats.instrs[s];
                    auto mnemonic = section.mnemonics.find(s);
                    instructions[(mnemonic != section.mnemonics.end()) ? mnemonic->second : ("slot " + std::to_string(s))] = Json::Value(stats.instrs[s]);
                }
                function["instructions_total"] = Json::Value(total);
                function["instructions"] = Json::Value(std::move(instructions));
            }
            functions.emplace_back(std::move(function));
        }
        Json::Value::Object stack;
        stack["index"] = Json::Value(index);
        stack["entry"] = Json::Value(section.entry);
        stack["functions"] = Json::Value(std::move(functions));
        stacks.emplace_back(std::move(stack));
    }
    Json::Value::Object root;
    root["version"] = Json::Value(1);
    root["time_unit"] = Json::Value("ns");
    root["stacks"] = Json::Value(std::move(stacks));
    return Json::Value(std::move(root));
}

// ';' separates frames and the last space the value: neither may appear in one.
static std::string folded_frame(const std::string& module, const std::string& func){
    std::string frame = frame_of(module, func);
    for(char& c : frame){
        if(c == ';' || c == ' '){
            c = '_';
        }
    }
    return frame;
}

std::string Profiler::folded() const {
    std::ostringstream out;
    for(const std::unique_ptr<Section>& section : sections){
        struct Walk {
            const Section& section;
            std::ostringstream& out;
            void operator()(const Node& node, const std::string& path){
                for(const auto& [funcaddr, child] : node.children){
                    auto named = section.names.find(funcaddr);
                    std::string frame = (named != section.names.end())
                        ? folded_frame(named->second.module, named->second.func)
                        : ("funcaddr_" + std::to_string(funcaddr));
                    std::string here = path.empty() ? frame : (path + ";" + frame);
                    out << here << " " << child->exclusive << "\n";
                    (*this)(*child, here);
                }
            }
        };
        Walk {*section, out}(section->root, "");
    }
    return out.str();
}

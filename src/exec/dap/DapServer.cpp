// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "DapServer.hpp"
#include "Framing.hpp"

#include <exception.hpp>
#include <dump/dump.hpp>

#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <sstream>

using namespace Dap;
using namespace WasmVM;

namespace {

using Object = Json::Value::Object;
using Array = Json::Value::Array;

Json::Value* member(Json::Value& value, const std::string& key){
    if(value.type() != Json::Value::Type::Object){
        return nullptr;
    }
    Object& object = value.get<Object>();
    auto found = object.find(key);
    return (found == object.end()) ? nullptr : &found->second;
}

std::optional<int64_t> as_int(Json::Value* value){
    if(value == nullptr){
        return std::nullopt;
    }
    if(value->type() == Json::Value::Type::Integer){
        return value->get<Json::Value::Integer>();
    }
    if(value->type() == Json::Value::Type::Number){
        return (int64_t)value->get<Json::Value::Number>();
    }
    return std::nullopt;
}

std::optional<bool> as_bool(Json::Value* value){
    if(value == nullptr || value->type() != Json::Value::Type::Bool){
        return std::nullopt;
    }
    return value->get<Json::Value::Bool>();
}

std::optional<std::string> as_string(Json::Value* value){
    if(value == nullptr || value->type() != Json::Value::Type::String){
        return std::nullopt;
    }
    return value->get<Json::Value::String>();
}

std::string base64(const byte_t* data, size_t size){
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((size + 2) / 3 * 4);
    for(size_t i = 0; i < size; i += 3){
        uint32_t chunk = (uint32_t)data[i] << 16;
        if(i + 1 < size) chunk |= (uint32_t)data[i + 1] << 8;
        if(i + 2 < size) chunk |= (uint32_t)data[i + 2];
        out += table[(chunk >> 18) & 0x3F];
        out += table[(chunk >> 12) & 0x3F];
        out += (i + 1 < size) ? table[(chunk >> 6) & 0x3F] : '=';
        out += (i + 2 < size) ? table[chunk & 0x3F] : '=';
    }
    return out;
}

// The shortest %g form that reads back as the same value.
template<typename F> std::string format_float(F value, int min_precision, int max_precision){
    char buf[64];
    for(int precision = min_precision; precision <= max_precision; ++precision){
        std::snprintf(buf, sizeof(buf), "%.*g", precision, (double)value);
        if((F)std::strtod(buf, nullptr) == value){
            break;
        }
    }
    return buf;
}

std::string format_pointer(const void* pointer){
    if(pointer == nullptr){
        return "null";
    }
    char buf[32];
    std::snprintf(buf, sizeof(buf), "0x%" PRIxPTR, (uintptr_t)pointer);
    return buf;
}

bool opens_block(Opcode::opcode_t opcode){
    return opcode == Opcode::Block || opcode == Opcode::Loop || opcode == Opcode::If || opcode == Opcode::Try_table;
}

std::string opcode_name(Opcode::opcode_t opcode){
    switch(opcode){
        case Opcode::Block: return "block";
        case Opcode::Loop: return "loop";
        case Opcode::If: return "if";
        case Opcode::Try_table: return "try_table";
        default: return "label";
    }
}

// "mem:<memaddr>"
std::optional<index_t> parse_memory_reference(const std::string& reference){
    if(reference.rfind("mem:", 0) != 0 || reference.size() == 4){
        return std::nullopt;
    }
    char* end = nullptr;
    unsigned long long addr = std::strtoull(reference.c_str() + 4, &end, 10);
    if(*end != '\0' || addr > (index_t)-1){
        return std::nullopt;
    }
    return (index_t)addr;
}

// Parses a new value of the same type as `old`: integers in decimal or 0x hex,
// floats as strtod reads them. References cannot be set.
std::optional<Value> parse_value(const std::string& text, const Value& old){
    const char* first = text.c_str();
    char* end = nullptr;
    errno = 0;
    switch(old.index()){
        case 0: { // i32
            long long parsed = std::strtoll(first, &end, 0);
            if(text.empty() || *end != '\0' || errno != 0){
                unsigned long long bits = std::strtoull(first, &end, 0);
                if(text.empty() || *end != '\0' || bits > 0xFFFFFFFFull){
                    return std::nullopt;
                }
                return Value((i32_t)(u32_t)bits);
            }
            if(parsed < INT32_MIN || parsed > (long long)UINT32_MAX){
                return std::nullopt;
            }
            return Value((i32_t)(u32_t)(int64_t)parsed);
        }
        case 1: { // i64
            long long parsed = std::strtoll(first, &end, 0);
            if(text.empty() || *end != '\0' || errno != 0){
                errno = 0;
                unsigned long long bits = std::strtoull(first, &end, 0);
                if(text.empty() || *end != '\0' || errno != 0 || text[0] == '-'){
                    return std::nullopt;
                }
                return Value((i64_t)bits);
            }
            return Value((i64_t)parsed);
        }
        case 2: case 3: {
            double parsed = std::strtod(first, &end);
            if(text.empty() || *end != '\0'){
                return std::nullopt;
            }
            if(old.index() == 2){
                return Value((f32_t)parsed);
            }
            return Value((f64_t)parsed);
        }
        default:
            return std::nullopt;
    }
}

}

std::string Server::value_type(const Value& value){
    static const char* names[] = {"i32", "i64", "f32", "f64", "funcref", "externref", "i31ref", "exnref", "structref", "arrayref"};
    return names[value.index()];
}

std::string Server::format_value(const Value& value){
    char buf[64];
    switch(value.index()){
        case 0: {
            i32_t v = std::get<i32_t>(value);
            std::snprintf(buf, sizeof(buf), "%" PRId32 " (0x%08" PRIx32 ")", v, (u32_t)v);
            return buf;
        }
        case 1: {
            i64_t v = std::get<i64_t>(value);
            std::snprintf(buf, sizeof(buf), "%" PRId64 " (0x%016" PRIx64 ")", v, (u64_t)v);
            return buf;
        }
        case 2:
            return format_float(std::get<f32_t>(value), 6, 9);
        case 3:
            return format_float(std::get<f64_t>(value), 15, 17);
        case 4: {
            const funcref_t& ref = std::get<funcref_t>(value);
            return ref ? ("func@" + std::to_string(ref.value())) : "null";
        }
        case 5:
            return format_pointer(std::get<externref_t>(value));
        case 6: {
            const i31ref_t& ref = std::get<i31ref_t>(value);
            return ref.value ? ("i31 " + std::to_string(ref.value.value())) : "null";
        }
        case 7:
            return format_pointer(std::get<exnref_t>(value).value.get());
        case 8:
            return format_pointer(std::get<structref_t>(value).get());
        case 9:
            return format_pointer(std::get<arrayref_t>(value).get());
    }
    return "?";
}

Server::Source Server::make_source(const WasmModule& module, index_t funcidx, const std::string& file){
    index_t import_funcs = 0;
    for(const WasmImport& import : module.imports){
        if(std::holds_alternative<index_t>(import.desc)){
            ++import_funcs;
        }
    }
    const WasmFunc& func = module.funcs[funcidx - import_funcs];
    Source source;
    source.funcidx = funcidx;
    source.opener.assign(func.body.size(), -1);

    DumpNames names(module, true);
    DumpNames::Scope scope(names);
    // Every definition counts as printed, so references print by name.
    auto define_all = [&](DumpSpace space, const WasmNames::NameMap& map){
        for(auto& entry : map){
            names.define(space, entry.first);
        }
    };
    define_all(DumpSpace::func, module.names.funcs);
    define_all(DumpSpace::table, module.names.tables);
    define_all(DumpSpace::mem, module.names.mems);
    define_all(DumpSpace::global, module.names.globals);
    define_all(DumpSpace::elem, module.names.elems);
    define_all(DumpSpace::data, module.names.datas);
    define_all(DumpSpace::tag, module.names.tags);
    define_all(DumpSpace::type, module.names.types);

    std::ostringstream text;
    const std::string* func_name = names.func(funcidx);
    source.name = file + ":" + (func_name ? ("$" + *func_name) : ("func " + std::to_string(funcidx)));
    names.begin_func(funcidx);
    // Line 1: the header.
    text << "(func";
    dump_def_name(text, func_name);
    text << " (type ";
    dump_ref(text, DumpSpace::type, func.typeidx) << ")";
    const FuncType* type = nullptr;
    if(func.typeidx < module.types.size() && module.types[func.typeidx].is_func()){
        type = &module.types[func.typeidx].func();
    }
    index_t params = type ? (index_t)type->params.size() : 0;
    for(index_t i = 0; i < params; ++i){
        text << " (param";
        dump_def_name(text, names.local(i));
        text << " " << type->params[i] << ")";
    }
    if(type){
        for(const ValueType& result : type->results){
            text << " (result " << result << ")";
        }
    }
    for(index_t i = 0; i < func.locals.size(); ++i){
        text << " (local";
        dump_def_name(text, names.local(params + i));
        text << " " << func.locals[i] << ")";
    }
    text << "\n";
    // Line k + 2: instruction k, indented by block nesting.
    std::vector<size_t> open;
    for(size_t k = 0; k < func.body.size(); ++k){
        const WasmInstr& instr = func.body[k];
        size_t depth = open.size();
        if(instr.opcode == Opcode::End){
            if(!open.empty()){
                source.opener[k] = (long)open.back();
                open.pop_back();
                depth = open.size();
            }
        }else if(instr.opcode == Opcode::Else && depth > 0){
            depth -= 1;
        }
        // The function's own `end` lines up with `(func`.
        bool function_end = (instr.opcode == Opcode::End) && (source.opener[k] < 0);
        text << std::string(function_end ? 0 : 2 * (depth + 1), ' ') << instr << "\n";
        if(opens_block(instr.opcode)){
            open.push_back(k);
        }
    }
    names.end_func();
    source.text = text.str();
    return source;
}

Server::Server(Connection connection, Store& store) : connection(std::move(connection)), store(store){}

Server::~Server(){
    connection.shutdown();
    if(thread.joinable()){
        thread.join();
    }
}

void Server::add_module(const std::filesystem::path& path, const WasmModule& module){
    Module& entry = modules.emplace_back();
    entry.path = path;
    entry.module = &module;
    for(const WasmImport& import : module.imports){
        if(std::holds_alternative<index_t>(import.desc)){
            ++entry.import_funcs;
        }else if(std::holds_alternative<GlobalType>(import.desc)){
            entry.global_mutable.push_back(std::get<GlobalType>(import.desc).mut == GlobalType::variable);
        }
    }
    for(const WasmGlobal& global : module.globals){
        entry.global_mutable.push_back(global.type.mut == GlobalType::variable);
    }
    std::string file = path.filename().string();
    for(index_t i = 0; i < module.funcs.size(); ++i){
        Source source = make_source(module, entry.import_funcs + i, file);
        source.module = modules.size() - 1;
        entry.sources.push_back(sources.size());
        sources.emplace_back(std::move(source));
    }
}

void Server::start(){
    thread = std::thread([this]{ serve(); });
}

bool Server::wait_configured(){
    std::unique_lock<std::mutex> lock(mutex);
    changed.wait(lock, [this]{ return configured || !connected; });
    return configured && !terminating;
}

void Server::before_instantiate(size_t index, ModuleInst& moduleinst){
    std::lock_guard<std::mutex> lock(mutex);
    if(terminating){
        throw Terminate();
    }
    current_module = index;
    modules[index].inst = &moduleinst;
    // Function addresses exist only once module_instanciate has allocated
    // them, which is after this returns but before the start function runs.
    // Stop at its first instruction to bind the breakpoints there.
    if(modules[index].module->start && !detached && !internal_pause){
        internal_pause = true;
        pause();
    }
}

void Server::after_instantiate(size_t index){
    std::lock_guard<std::mutex> lock(mutex);
    bind(index);
}

void Server::bind(size_t index){
    Module& module = modules[index];
    if(module.bound || module.inst == nullptr){
        return;
    }
    if(module.inst->funcaddrs.size() < module.import_funcs + module.sources.size()){
        return; // not allocated yet
    }
    module.bound = true;
    for(size_t i = 0; i < module.sources.size(); ++i){
        Source& source = sources[module.sources[i]];
        source.funcaddr = module.inst->funcaddrs[module.import_funcs + i];
        for(index_t instr : source.breakpoints){
            set_breakpoint({source.funcaddr.value(), instr});
        }
    }
}

void Server::apply_breakpoints(Source& source, const std::set<index_t>& previous){
    if(!source.funcaddr){
        return;
    }
    for(index_t instr : previous){
        if(!source.breakpoints.contains(instr)){
            clear_breakpoint({source.funcaddr.value(), instr});
        }
    }
    for(index_t instr : source.breakpoints){
        set_breakpoint({source.funcaddr.value(), instr});
    }
}

std::optional<size_t> Server::source_of(index_t funcaddr) const {
    for(size_t i = 0; i < sources.size(); ++i){
        if(sources[i].funcaddr == funcaddr){
            return i;
        }
    }
    return std::nullopt;
}

const Server::Module* Server::module_of(const ModuleInst& inst) const {
    for(const Module& module : modules){
        if(module.inst == &inst){
            return &module;
        }
    }
    return nullptr;
}

Debugger::Resume Server::on_stop(const Stop& stop){
    std::unique_lock<std::mutex> lock(mutex);
    if(terminating){
        throw Terminate();
    }
    if(detached || !connected){
        return Resume::Continue;
    }
    switch(stop.reason){
        case Reason::Trap:
            if(!stop_on_trap){
                return Resume::Continue;
            }
            return wait_for_resume(stop, "exception", stop.trap ? stop.trap->what() : "trap");
        case Reason::Exception:
            if(!stop_on_uncaught){
                return Resume::Continue;
            }
            return wait_for_resume(stop, "exception", "uncaught exception");
        case Reason::Breakpoint:
            return wait_for_resume(stop, "breakpoint", "");
        case Reason::Step:
            return wait_for_resume(stop, "step", "");
        case Reason::Pause: {
            // Layer 1 has one pause flag: ours and the client's may coincide,
            // and a pause outranks a breakpoint or a step at the same place.
            bool internal = internal_pause;
            internal_pause = false;
            bind(current_module);
            if(pause_requested){
                pause_requested = false;
                return wait_for_resume(stop, "pause", "");
            }
            if(internal && stop_on_entry && !entry_reported){
                entry_reported = true;
                return wait_for_resume(stop, "entry", "");
            }
            entry_reported = true;
            if(stepping){
                // The step the client asked for carried over into this
                // start function; Layer 1 would have stopped here unless it
                // was a step out of a depth-1 frame, which has no caller.
                bool stops = (stepping.value() != Resume::StepOut) || (stop.stack.frames.size() < step_depth);
                if(stops){
                    return wait_for_resume(stop, "step", "");
                }
            }
            if(source_of(stop.position.funcaddr)){
                Source& source = sources[source_of(stop.position.funcaddr).value()];
                if(source.breakpoints.contains(stop.position.instr)){
                    return wait_for_resume(stop, "breakpoint", "");
                }
            }
            return Resume::Continue;
        }
    }
    return Resume::Continue;
}

Debugger::Resume Server::wait_for_resume(const Stop& stop, const std::string& reason, const std::string& text){
    // Called with `mutex` held, on the VM thread.
    stopped = &stop;
    resume.reset();
    var_refs.clear();
    stepping.reset();
    Object body {
        {"reason", reason},
        {"threadId", 1},
        {"allThreadsStopped", true},
    };
    if(!text.empty()){
        body["description"] = text;
        body["text"] = text;
    }
    event("stopped", body);
    std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
    changed.wait(lock, [this]{ return resume.has_value() || terminating || detached || !connected; });
    lock.release();
    stopped = nullptr;
    var_refs.clear();
    if(terminating){
        throw Terminate();
    }
    if(!resume || detached || !connected){
        return Resume::Continue;
    }
    Resume action = resume.value();
    if(action != Resume::Continue){
        stepping = action;
        step_depth = stop.stack.frames.size();
    }
    return action;
}

void Server::finish(int exit_code){
    {
        std::unique_lock<std::mutex> lock(mutex);
        if(connected && !detached){
            lock.unlock();
            event("exited", Object {{"exitCode", exit_code}});
            event("terminated");
            lock.lock();
            // The client answers terminated with disconnect.
            changed.wait(lock, [this]{ return !connected; });
        }
    }
    connection.shutdown();
    if(thread.joinable()){
        thread.join();
    }
}

void Server::send(Object message){
    std::lock_guard<std::mutex> lock(write_mutex);
    message["seq"] = seq++;
    std::ostringstream out;
    out << Json::newline(false) << Json::Value(std::move(message));
    connection.write(frame(out.str()));
}

void Server::event(const std::string& name, Json::Value body){
    Object message {{"type", "event"}, {"event", name}};
    if(body.type() != Json::Value::Type::Object || !body.get<Object>().empty()){
        message["body"] = std::move(body);
    }
    send(std::move(message));
}

void Server::respond(Json::Value& request, Json::Value body){
    Object message {
        {"type", "response"},
        {"request_seq", as_int(member(request, "seq")).value_or(0)},
        {"success", true},
        {"command", as_string(member(request, "command")).value_or("")},
    };
    if(body.type() != Json::Value::Type::Object || !body.get<Object>().empty()){
        message["body"] = std::move(body);
    }
    send(std::move(message));
}

void Server::fail(Json::Value& request, const std::string& text){
    send(Object {
        {"type", "response"},
        {"request_seq", as_int(member(request, "seq")).value_or(0)},
        {"success", false},
        {"command", as_string(member(request, "command")).value_or("")},
        {"message", text},
    });
}

void Server::serve(){
    MessageReader reader;
    char buffer[4096];
    bool open = true;
    while(open){
        size_t got = connection.read(buffer, sizeof(buffer));
        if(got == 0){
            break;
        }
        reader.feed(buffer, got);
        try {
            while(std::optional<std::string> body = reader.next()){
                Json::Value request;
                std::istringstream in(body.value());
                in >> request;
                if(as_string(member(request, "type")) != "request"){
                    continue;
                }
                handle(request);
                std::lock_guard<std::mutex> lock(mutex);
                if(!connected){
                    open = false;
                    break;
                }
            }
        }catch(Exception::Exception& e){
            // A malformed message ends the session: framing cannot recover.
            break;
        }
    }
    std::lock_guard<std::mutex> lock(mutex);
    if(connected && !detached){
        // The client went away without disconnect: launch semantics, end the debuggee.
        terminating = true;
        pause();
    }
    connected = false;
    changed.notify_all();
}

void Server::disconnect(bool terminate){
    // Called with `mutex` held.
    if(terminate){
        terminating = true;
        pause(); // a running guest stops at its next instruction and ends there
    }else{
        detached = true;
        clear_breakpoints();
        stop_on_trap = false;
        stop_on_uncaught = false;
    }
    connected = false;
    changed.notify_all();
}

std::vector<const Frame*> Server::frames() const {
    std::vector<const Frame*> result;
    if(stopped){
        for(auto it = stopped->stack.frames.rbegin(); it != stopped->stack.frames.rend(); ++it){
            result.push_back(&*it);
        }
    }
    return result;
}

// The instruction a frame is at: the stop position for the top frame, the
// call instruction for the others (their pc has moved past it). -1 when none.
long Server::frame_instr(size_t index) const {
    std::vector<const Frame*> all = frames();
    const Frame* frame = all[index];
    if(!std::holds_alternative<WasmFunc>(store.funcs[frame->funcaddr].body)){
        return -1;
    }
    if(index == 0){
        return (stopped->position.instr == (index_t)-1) ? -1 : (long)stopped->position.instr;
    }
    if(frame->labels.empty() || !frame->labels.top().pc){
        return -1;
    }
    return (long)frame->labels.top().pc->current - 1;
}

Json::Value Server::stack_trace(){
    Array result;
    std::vector<const Frame*> all = frames();
    for(size_t i = 0; i < all.size(); ++i){
        const Frame* frame = all[i];
        Object entry {{"id", (int64_t)i + 1}, {"column", 0}, {"line", 0}};
        std::optional<size_t> found = source_of(frame->funcaddr);
        long instr = frame_instr(i);
        if(found){
            Source& source = sources[found.value()];
            entry["name"] = source.name.substr(source.name.find(':') + 1);
            entry["source"] = Object {
                {"name", source.name},
                {"sourceReference", (int64_t)found.value() + 1},
            };
            if(instr >= 0){
                entry["line"] = (int64_t)instr + (lines_start_at_1 ? 2 : 1);
                entry["column"] = lines_start_at_1 ? 1 : 0;
            }
        }else{
            entry["name"] = std::string("host function @") + std::to_string(frame->funcaddr);
            entry["presentationHint"] = "subtle";
        }
        result.emplace_back(std::move(entry));
    }
    return Object {{"stackFrames", result}, {"totalFrames", (int64_t)result.size()}};
}

Json::Value Server::scopes(size_t frame){
    Array result;
    std::vector<const Frame*> all = frames();
    if(std::holds_alternative<WasmFunc>(store.funcs[all[frame]->funcaddr].body)){
        auto add = [&](const char* name, VarRef::Kind kind, bool expensive){
            var_refs.push_back({kind, frame});
            result.emplace_back(Object {
                {"name", name},
                {"variablesReference", (int64_t)var_refs.size()},
                {"expensive", expensive},
            });
        };
        add("Locals", VarRef::Locals, false);
        add("Operand stack", VarRef::Operands, false);
        add("Globals", VarRef::Globals, false);
        add("Memory", VarRef::Memory, false);
    }
    return Object {{"scopes", result}};
}

Json::Value Server::variables(const VarRef& ref){
    Array result;
    const Frame* frame = frames()[ref.frame];
    const ModuleInst& inst = frame->module;
    std::optional<size_t> found = source_of(frame->funcaddr);
    auto entry = [](std::string name, const Value& value){
        return Object {
            {"name", std::move(name)},
            {"value", Server::format_value(value)},
            {"type", Server::value_type(value)},
            {"variablesReference", 0},
        };
    };
    auto name_or = [](const WasmNames::NameMap* map, index_t idx, const std::string& fallback){
        if(map){
            auto it = map->find(idx);
            if(it != map->end()){
                return "$" + it->second;
            }
        }
        return fallback + " " + std::to_string(idx);
    };
    switch(ref.kind){
        case VarRef::Locals: {
            const WasmNames::NameMap* names = nullptr;
            if(found){
                auto it = inst.names.locals.find(sources[found.value()].funcidx);
                if(it != inst.names.locals.end()){
                    names = &it->second;
                }
            }
            for(index_t i = 0; i < frame->locals.size(); ++i){
                result.emplace_back(entry(name_or(names, i, "local"), frame->locals[i]));
            }
        }break;
        case VarRef::Operands: {
            index_t i = 0;
            for(const Label& label : frame->labels){
                std::string kind = "label";
                if(found && label.pc){
                    const Source& source = sources[found.value()];
                    const WasmFunc& func = std::get<WasmFunc>(store.funcs[frame->funcaddr].body);
                    size_t continuation = label.pc->continuation;
                    if(continuation < func.body.size()){
                        if(func.body[continuation].opcode == Opcode::Loop){
                            kind = "loop";
                        }else if(func.body[continuation].opcode == Opcode::End){
                            long opener = source.opener[continuation];
                            kind = (opener < 0) ? "func" : opcode_name(func.body[opener].opcode);
                        }
                    }
                }
                size_t count = (size_t)std::distance(label.values.begin(), label.values.end());
                var_refs.push_back({VarRef::Label, ref.frame, i});
                result.emplace_back(Object {
                    {"name", "label " + std::to_string(i) + " (" + kind + ")"},
                    {"value", "arity " + std::to_string(label.arity) + ", " + std::to_string(count) + " value" + (count == 1 ? "" : "s")},
                    {"variablesReference", (int64_t)var_refs.size()},
                    {"indexedVariables", (int64_t)count},
                });
                ++i;
            }
        }break;
        case VarRef::Label: {
            auto label = frame->labels.begin();
            std::advance(label, ref.label);
            index_t i = 0;
            for(const Value& value : label->values){
                result.emplace_back(entry("[" + std::to_string(i++) + "]", value));
            }
        }break;
        case VarRef::Globals: {
            const Module* module = module_of(inst);
            for(index_t i = 0; i < inst.globaladdrs.size(); ++i){
                const GlobalInst& global = store.globals[inst.globaladdrs[i]];
                Object item = entry(name_or(&inst.names.globals, i, "global"), global.value);
                bool writable = module && i < module->global_mutable.size() && module->global_mutable[i];
                if(!writable){
                    item["presentationHint"] = Object {{"attributes", Array {Json::Value("readOnly")}}};
                }
                result.emplace_back(std::move(item));
            }
        }break;
        case VarRef::Memory: {
            for(index_t i = 0; i < inst.memaddrs.size(); ++i){
                index_t addr = inst.memaddrs[i];
                size_t bytes = store.mems[addr].data.size();
                result.emplace_back(Object {
                    {"name", name_or(&inst.names.mems, i, "memory")},
                    {"value", std::to_string(bytes / 65536) + " pages (" + std::to_string(bytes) + " bytes)"},
                    {"type", "memory"},
                    {"variablesReference", 0},
                    {"memoryReference", "mem:" + std::to_string(addr)},
                });
            }
        }break;
    }
    return Object {{"variables", result}};
}

void Server::handle(Json::Value& request){
    std::string command = as_string(member(request, "command")).value_or("");
    Json::Value empty;
    Json::Value* arguments = member(request, "arguments");
    if(arguments == nullptr){
        arguments = &empty;
    }
    std::unique_lock<std::mutex> lock(mutex);

    if(command == "initialize"){
        lines_start_at_1 = as_bool(member(*arguments, "linesStartAt1")).value_or(true);
        lock.unlock();
        respond(request, Object {
            {"supportsConfigurationDoneRequest", true},
            {"supportsLoadedSourcesRequest", true},
            {"supportsReadMemoryRequest", true},
            {"supportsSetVariable", true},
            {"supportTerminateDebuggee", true},
            {"exceptionBreakpointFilters", Array {
                Object {{"filter", "trap"}, {"label", "Trap"}, {"default", true}},
                Object {{"filter", "uncaught"}, {"label", "Uncaught exception"}, {"default", true}},
            }},
        });
        event("initialized");
    }else if(command == "launch"){
        stop_on_entry = as_bool(member(*arguments, "stopOnEntry")).value_or(false);
        launched = true;
        lock.unlock();
        respond(request);
        for(size_t i = 0; i < sources.size(); ++i){
            event("loadedSource", Object {
                {"reason", "new"},
                {"source", Object {{"name", sources[i].name}, {"sourceReference", (int64_t)i + 1}}},
            });
        }
    }else if(command == "configurationDone"){
        if(!launched){
            lock.unlock();
            fail(request, "launch first");
            return;
        }
        configured = true;
        changed.notify_all();
        lock.unlock();
        respond(request);
    }else if(command == "setBreakpoints"){
        Json::Value* source_arg = member(*arguments, "source");
        std::optional<int64_t> reference = source_arg ? as_int(member(*source_arg, "sourceReference")) : std::nullopt;
        if(!reference || reference.value() < 1 || (size_t)reference.value() > sources.size()){
            lock.unlock();
            fail(request, "unknown source");
            return;
        }
        Source& source = sources[reference.value() - 1];
        const WasmFunc& func = modules[source.module].module->funcs[source.funcidx - modules[source.module].import_funcs];
        std::set<index_t> previous = source.breakpoints;
        source.breakpoints.clear();
        Array result;
        Json::Value* list = member(*arguments, "breakpoints");
        if(list && list->type() == Json::Value::Type::Array){
            for(Json::Value& item : list->get<Array>()){
                int64_t line = as_int(member(item, "line")).value_or(0);
                int64_t instr = line - (lines_start_at_1 ? 2 : 1);
                if(instr >= 0 && (size_t)instr < func.body.size()){
                    source.breakpoints.insert((index_t)instr);
                    result.emplace_back(Object {{"verified", true}, {"line", line}});
                }else{
                    result.emplace_back(Object {{"verified", false}, {"line", line}, {"message", "not an instruction line"}});
                }
            }
        }
        apply_breakpoints(source, previous);
        lock.unlock();
        respond(request, Object {{"breakpoints", result}});
    }else if(command == "setExceptionBreakpoints"){
        stop_on_trap = false;
        stop_on_uncaught = false;
        Json::Value* filters = member(*arguments, "filters");
        if(filters && filters->type() == Json::Value::Type::Array){
            for(Json::Value& filter : filters->get<Array>()){
                std::optional<std::string> name = as_string(&filter);
                stop_on_trap |= (name == "trap");
                stop_on_uncaught |= (name == "uncaught");
            }
        }
        lock.unlock();
        respond(request);
    }else if(command == "threads"){
        lock.unlock();
        respond(request, Object {{"threads", Array {Object {{"id", 1}, {"name", "main"}}}}});
    }else if(command == "loadedSources"){
        Array result;
        for(size_t i = 0; i < sources.size(); ++i){
            result.emplace_back(Object {{"name", sources[i].name}, {"sourceReference", (int64_t)i + 1}});
        }
        lock.unlock();
        respond(request, Object {{"sources", result}});
    }else if(command == "source"){
        std::optional<int64_t> reference = as_int(member(*arguments, "sourceReference"));
        if(Json::Value* source_arg = member(*arguments, "source")){
            if(auto inner = as_int(member(*source_arg, "sourceReference"))){
                reference = inner;
            }
        }
        if(!reference || reference.value() < 1 || (size_t)reference.value() > sources.size()){
            lock.unlock();
            fail(request, "unknown source");
            return;
        }
        std::string text = sources[reference.value() - 1].text;
        lock.unlock();
        respond(request, Object {{"content", text}, {"mimeType", "text/x-wasm"}});
    }else if(command == "pause"){
        if(stopped == nullptr){
            pause_requested = true;
            pause();
        }
        lock.unlock();
        respond(request);
    }else if(command == "continue" || command == "next" || command == "stepIn" || command == "stepOut"){
        if(stopped == nullptr || resume){
            lock.unlock();
            fail(request, "not stopped");
            return;
        }
        Resume action = (command == "continue") ? Resume::Continue
            : (command == "next") ? Resume::Next
            : (command == "stepIn") ? Resume::StepIn : Resume::StepOut;
        lock.unlock();
        // The response goes out before the VM moves on, so it precedes the next stop.
        if(command == "continue"){
            respond(request, Object {{"allThreadsContinued", true}});
        }else{
            respond(request);
        }
        lock.lock();
        resume = action;
        changed.notify_all();
    }else if(command == "stackTrace"){
        if(stopped == nullptr){
            lock.unlock();
            fail(request, "not stopped");
            return;
        }
        Json::Value body = stack_trace();
        lock.unlock();
        respond(request, body);
    }else if(command == "scopes"){
        int64_t frame = as_int(member(*arguments, "frameId")).value_or(0);
        if(stopped == nullptr || frame < 1 || (size_t)frame > stopped->stack.frames.size()){
            lock.unlock();
            fail(request, "no such frame");
            return;
        }
        Json::Value body = scopes((size_t)frame - 1);
        lock.unlock();
        respond(request, body);
    }else if(command == "variables"){
        int64_t reference = as_int(member(*arguments, "variablesReference")).value_or(0);
        if(stopped == nullptr || reference < 1 || (size_t)reference > var_refs.size()){
            lock.unlock();
            fail(request, "no such variables reference");
            return;
        }
        Json::Value body = variables(var_refs[reference - 1]);
        lock.unlock();
        respond(request, body);
    }else if(command == "setVariable"){
        int64_t reference = as_int(member(*arguments, "variablesReference")).value_or(0);
        std::string name = as_string(member(*arguments, "name")).value_or("");
        std::string text = as_string(member(*arguments, "value")).value_or("");
        if(stopped == nullptr || reference < 1 || (size_t)reference > var_refs.size()
            || var_refs[reference - 1].kind != VarRef::Globals){
            lock.unlock();
            fail(request, "only globals can be set");
            return;
        }
        const ModuleInst& inst = frames()[var_refs[reference - 1].frame]->module;
        const Module* module = module_of(inst);
        std::optional<index_t> index;
        for(index_t i = 0; i < inst.globaladdrs.size(); ++i){
            auto named = inst.names.globals.find(i);
            std::string shown = (named != inst.names.globals.end()) ? ("$" + named->second) : ("global " + std::to_string(i));
            if(shown == name){
                index = i;
                break;
            }
        }
        if(!index || !module || index.value() >= module->global_mutable.size() || !module->global_mutable[index.value()]){
            lock.unlock();
            fail(request, "not a mutable global");
            return;
        }
        GlobalInst& global = store.globals[inst.globaladdrs[index.value()]];
        std::optional<Value> parsed = parse_value(text, global.value);
        if(!parsed){
            lock.unlock();
            fail(request, "cannot parse '" + text + "' as " + value_type(global.value));
            return;
        }
        global.value = parsed.value();
        Object body {{"value", format_value(global.value)}, {"type", value_type(global.value)}};
        lock.unlock();
        respond(request, body);
    }else if(command == "readMemory"){
        std::optional<index_t> addr = parse_memory_reference(as_string(member(*arguments, "memoryReference")).value_or(""));
        int64_t offset = as_int(member(*arguments, "offset")).value_or(0);
        int64_t count = as_int(member(*arguments, "count")).value_or(0);
        if(stopped == nullptr || !addr || addr.value() >= store.mems.size() || offset < 0 || count < 0){
            lock.unlock();
            fail(request, "cannot read memory");
            return;
        }
        const std::vector<byte_t>& data = store.mems[addr.value()].data;
        uint64_t start = (uint64_t)offset;
        uint64_t available = (start >= data.size()) ? 0 : std::min<uint64_t>((uint64_t)count, data.size() - start);
        char address[32];
        std::snprintf(address, sizeof(address), "0x%" PRIx64, start);
        Object body {
            {"address", std::string(address)},
            {"data", base64(data.data() + (available ? start : 0), (size_t)available)},
        };
        if((uint64_t)count > available){
            body["unreadableBytes"] = (int64_t)((uint64_t)count - available);
        }
        lock.unlock();
        respond(request, body);
    }else if(command == "disconnect"){
        bool terminate = as_bool(member(*arguments, "terminateDebuggee")).value_or(true);
        lock.unlock();
        respond(request);
        lock.lock();
        disconnect(terminate);
    }else{
        lock.unlock();
        fail(request, "unsupported request '" + command + "'");
    }
}

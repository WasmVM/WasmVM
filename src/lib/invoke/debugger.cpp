// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Debugger Layer 1 (#53). Built only under WASMVM_DEBUG (src/lib/CMakeLists.txt).

#include <Debugger.hpp>
#include <instances/Stack.hpp>

using namespace WasmVM;

// Out of line: the key function, so the typeinfo lives once, in the library
// (see the note in <exception.hpp>).
Debugger::~Debugger() = default;

void Debugger::set_breakpoint(Position position){
    std::lock_guard<std::mutex> lock(breakpoints_mutex);
    breakpoints.insert(position);
}

void Debugger::clear_breakpoint(Position position){
    std::lock_guard<std::mutex> lock(breakpoints_mutex);
    breakpoints.erase(position);
}

void Debugger::clear_breakpoints(){
    std::lock_guard<std::mutex> lock(breakpoints_mutex);
    breakpoints.clear();
}

void Debugger::pause(){
    paused.store(true);
}

void Debugger::before_instr(const Stack& stack, Position position){
    const size_t depth = stack.frames.size();
    bool stop = false;
    Reason reason = Reason::Step;
    if(paused.load(std::memory_order_relaxed) && paused.exchange(false)){
        stop = true;
        reason = Reason::Pause;
    }else{
        std::lock_guard<std::mutex> lock(breakpoints_mutex);
        if(breakpoints.contains(position)){
            stop = true;
            reason = Reason::Breakpoint;
        }
    }
    if(!stop){
        switch(step){
            case Resume::Continue: break;
            case Resume::StepIn:   stop = true; break;
            case Resume::Next:     stop = (depth <= step_depth); break;
            case Resume::StepOut:  stop = (depth < step_depth); break;
        }
    }
    if(stop){
        step = on_stop(Stop {reason, position, stack, nullptr, nullptr});
        step_depth = depth;
    }
}

void Debugger::trapped(const Stack& stack, Position position, const Exception::Trap& trap){
    on_stop(Stop {Reason::Trap, position, stack, &trap, nullptr});
}

void Debugger::uncaught(const Stack& stack, Position position, GcPtr<WasmException> exn){
    on_stop(Stop {Reason::Exception, position, stack, nullptr, exn});
}

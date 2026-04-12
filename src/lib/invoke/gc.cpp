// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.
//
// GC struct and array instructions - stub implementations
// Full GC type system support pending

#include <structures/WasmInstr.hpp>
#include "RunVisitor.hpp"
#include "exception.hpp"

using namespace WasmVM;

// GC Struct operations (stub - GC type system not yet implemented)
void RunVisitor::operator()(Instr::Struct_new&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Struct_new_default&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Struct_get&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Struct_get_s&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Struct_get_u&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Struct_set&){ throw Exception::unreachable(); }

// GC Array operations (stub - GC type system not yet implemented)
void RunVisitor::operator()(Instr::Array_new&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_new_default&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_new_fixed&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_new_data&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_new_elem&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_get&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_get_s&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_get_u&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_set&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_len&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_fill&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_copy&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_init_data&){ throw Exception::unreachable(); }
void RunVisitor::operator()(Instr::Array_init_elem&){ throw Exception::unreachable(); }

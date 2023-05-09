// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "decode.hpp"
#include "exception.hpp"

#include <Util.hpp>
#include <variant>

using namespace WasmVM;
using namespace Decode;

#define AtomicInstr(I) \
    case Opcode::I : \
        instr.emplace<Instr::I>(); \
    break;
#define OneIndexInstr(I) \
    case Opcode::I : \
        instr.emplace<Instr::I>(stream.get<index_t>()); \
    break;

template<> Opcode::opcode_t Stream::get<Opcode::opcode_t>(){
    Opcode::opcode_t opcode = (Opcode::opcode_t) get<byte_t>();
    if(opcode > 0xff){
        opcode = (opcode << 8) | (Opcode::opcode_t) get<byte_t>();
    }
    return opcode;
}

template<> Stream& Decode::operator>> <ConstInstr>(Stream& stream, ConstInstr& instr){
    switch(stream.get<Opcode::opcode_t>()){
        OneIndexInstr(Ref_func)
        OneIndexInstr(Global_get)
        case Opcode::Ref_null :
            instr.emplace<Instr::Ref_null>(stream.get<RefType>());
        break;
        case Opcode::I32_const :
            instr.emplace<Instr::I32_const>(stream.get<i32_t>());
        break;
        case Opcode::I64_const :
            instr.emplace<Instr::I64_const>(stream.get<i64_t>());
        break;
        case Opcode::F32_const :
            instr.emplace<Instr::F32_const>(stream.get<f32_t>());
        break;
        case Opcode::F64_const :
            instr.emplace<Instr::F64_const>(stream.get<f64_t>());
        break;
        default:
            throw Exception::unknown_instruction(stream.location());
    }
    return stream;
}

#undef AtomicInstr

grammar instruction;

options {
    language = Cpp;
}

import token, types;

instr : plaininstr | blockinstr | foldedinstr;

label : Id?;

blocktype : result? | typeuse;

blockinstr : 'block' label blocktype instr* 'end' Id?
    | 'loop' label blocktype instr* 'end' Id?
    | 'if' label blocktype instr* ('else' Id? instr*) 'end' Id?
    ;

foldedinstr : '(' plaininstr foldedinstr* ')'
    | '(' 'block' label blocktype instr* ')'
    | '(' 'loop' label blocktype instr* ')'
    | '(' 'if' label blocktype foldedinstr* '(' 'then' instr* ')' ('(' 'else' instr* ')')? ')'
    ;

constexpr : 'i32.const' i32
    | 'i64.const' i64
    | 'f32.const' f32
    | 'f64.const' f64
    | 'ref.null' i32
    | 'ref.func' u32
    | 'global.get' i32
    ;

plaininstr : constexpr
    | controlinstr
    ;

controlinstr :
    | 'unreachable'
    | 'nop'
    | 'br' labelidx
    | 'br_if' labelidx
    | 'br_table' labelidx+
    | 'return'
    | 'call' funcidx
    | 'call_indirect' tableidx typeuse
    ;
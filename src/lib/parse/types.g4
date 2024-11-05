grammar types;

options {
    language = Cpp;
}

import token;

i32 : Integer;
i64 : Integer;
u32 : Integer;
u64 : Integer;
f32 : Float;
f64 : Float;

NumType : [if] ('32'|'64');
RefType : 'funcref' | 'externref';
valtype : NumType | RefType;
param : '(' 'param' (Id valtype | valtype*) ')' ;
result : '(' 'result' valtype* ')' ;
functype : '(' 'func' param* result* ')' ;
limits : u64 u64? ;
memtype : limits ;
tabletype : limits RefType ;
globaltype : valtype | '(' 'mut' valtype ')' ;

typeidx : u32 | Id ;
funcidx : u32 | Id ;
tableidx : u32 | Id ;
memidx : u32 | Id ;
globalidx : u32 | Id ;
elemidx : u32 | Id ;
dataidx : u32 | Id ;
localidx : u32 | Id ;
labelidx : u32 | Id ;

typeuse : ('(' 'type' typeidx ')')? param* result* ;
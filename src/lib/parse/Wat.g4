grammar Wat;

options {
    language = Cpp;
}

import token;

/* Module */
module : '(' 'module' modulefield* ')' | modulefield* ;

modulefield : type;

/* Sections */
type : '(' 'type' Id? functype ')' ;

/** Types **/
NumType : [if] ('32'|'64');
RefType : 'funcref' | 'externref';
HeapType : 'func' | 'extern';
valtype : RefType | HeapType;
param : '(' 'param' (Id valtype | valtype*) ')' ;
result : '(' 'result' valtype* ')' ;
functype : '(' 'func' param* result* ')' ;
limits : Integer Integer? ;
memtype : limits ;
tabletype : limits RefType ;
globaltype : valtype | '(' 'mut' valtype ')' ;

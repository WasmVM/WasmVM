grammar Wat;

options {
    language = Cpp;
}

import token, types, instructions;

/* Module */
module : '(' ('module' Id? modulefield*)? ')' | modulefield*;

modulefield : typesection
    | importsection
    | funcsection
    | tablesection
    | memorysection
    | globalsection
    | exportsection
    | startsection
    | elemsection
    | datasection
    ;

/* Sections */
typesection : '(' 'type' Id? functype ')' ;

importdesc : '(' 'func' Id? typeuse ')'
    | '(' 'table' Id? tabletype ')'
    | '(' 'memory' Id? memtype ')'
    | '(' 'global' Id? globaltype ')'
    ;
importabbr : '(' 'import' String String ')' ;
importsection : '(' importabbr importdesc ')' ;

funcsection : '(' 'func' Id? exportabbr* (importabbr typeuse | typeuse local* instr*) ')' ;
local : '(' 'local' (Id? valtype | valtype+) ')';

tablesection : '(' 'table' Id? exportabbr* (importabbr? tabletype
    | RefType '(' 'elem' (elemexpr* | funcidx*) ')'
    ) ')';

memorysection : '(' 'memory' Id? exportabbr* (importabbr? memtype | '(' 'data' String* ')') ')';

globalsection : '(' 'global' Id? exportabbr* (importabbr globaltype | globaltype constexpr) ')';

exportsection : '(' 'export' String exportdesc ')' ;
exportdesc : '(' 'func' funcidx ')'
    | '(' 'table' tableidx ')'
    | '(' 'memory' memidx ')'
    | '(' 'global' globalidx ')'
    ;
exportabbr : '(' 'export' String ')' ;

startsection : '(' 'start' funcidx ')' ;

elemsection : '(' 'elem' Id? (
    (tableuse? '(' 'offset'? constexpr ')' | 'declare')? elemlist
    | '(' 'offset'? constexpr ')' funcidx*
    ) ')' ;
elemlist : RefType elemexpr*
    | 'func' funcidx*
    ;
elemexpr : '(' 'item' constexpr ')' | '(' constexpr ')';
tableuse : '(' 'table' tableidx ')';

datasection : '(' 'data' Id? (memuse? '(' 'offset'? constexpr ')')? String* ')';
memuse : '(' 'memory' memidx ')';
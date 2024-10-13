grammar Wat;

options {
    language = Cpp;
}

import token, types, instructions;

/* Module */
module : '(' ('module' Id? modulefield*)? ')' | modulefield*;

modulefield : type;

/* Sections */
type : '(' 'type' Id? functype ')' ;

lexer grammar token;

options {
    language = Cpp;
}

/* Comment & WhiteSpace */
BlockComment : '(;' (BlockComment | .)*? ';)' -> skip;
LineComment : ';;' .*? '\n' -> skip;
WhiteSpace : [ \r\t\n]+ -> skip;

/* Integers */
fragment Sign : [+\-]?;
fragment Digit : [0-9];
fragment HexDigit : [0-9a-fA-F];
fragment Num : Digit '_'? Num | Digit;
fragment HexNum : HexDigit '_'? HexNum | HexDigit;
Integer : Sign (Num | '0x' HexNum);

/* Floats */
fragment Fmag : 'inf'
    | 'nan:0x' HexNum
    | 'nan'
    | Num '.' Num ([Ee] Sign Num)?
    | Num '.'? ([Ee] Sign Num)?
    | '0x' HexNum '.' HexNum ([Pp] Sign HexNum)?
    | '0x' HexNum '.'? ([Pp] Sign HexNum)?
    ;
Float : Sign Fmag;

/* String */
fragment StringElem : [\u0020\u0021\u0023-\u005b\u005d-\u007e\u0080-\uffff]
    | '\\t'
    | '\\n'
    | '\\r'
    | '\\"'
    | '\\\''
    | '\\\\'
    | '\\' HexDigit HexDigit
    | '\\u{' HexNum '}'
    ;
String : '"' StringElem*? '"';

/* Identifier */
Id : '$' [0-9a-zA-Z!#$%&'*+\-./:<=>?@\\^_`|~]+;
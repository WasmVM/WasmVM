grammar instructions;

options {
    language = Cpp;
}

import token, types;

instr : plaininstr | blockinstr | foldedinstr;

label : Id?;

elseinstr : instr;
blockinstr : 'block' label typeuse instr* 'end' Id?
    | 'loop' label typeuse instr* 'end' Id?
    | 'if' label typeuse instr* ('else' Id? elseinstr*)? 'end' Id?
    ;

foldedinstr : '(' plaininstr foldedinstr* ')'
    | '(' 'block' label typeuse instr* ')'
    | '(' 'loop' label typeuse instr* ')'
    | '(' 'if' label typeuse foldedinstr* '(' 'then' instr* ')' ('(' 'else' elseinstr* ')')? ')'
    ;

constexpr : 'i32.const' i32
    | 'i64.const' i64
    | 'f32.const' f32
    | 'f64.const' f64
    | RefNull
    | 'ref.func' funcidx
    | 'global.get' globalidx
    ;

plaininstr : controlinstr
    | referenceinstr
    | parametricinstr
    | variableinstr
    | tableinstr
    | memoryinstr
	| numericinstr
    ;

controlinstr : 'unreachable'
    | 'nop'
    | 'br' labelidx
    | 'br_if' labelidx
    | 'br_table' labelidx+
    | 'return'
    | 'call' funcidx
    | 'call_indirect' tableidx? typeuse
    ;

RefNull : 'ref.null' (BlockComment | LineComment | WhiteSpace)+ ('func' | 'extern');
referenceinstr : RefNull
    | 'ref.is_null'
    | 'ref.func' funcidx
    ;

parametricinstr : 'drop' | 'select' result* ;

variableinstr : 'local.get' localidx
    | 'local.set' localidx
    | 'local.tee' localidx
    | 'global.get' globalidx
    | 'global.set' globalidx
    ;

tableinstr : 'table.get' tableidx?
    | 'table.set' tableidx?
    | 'table.size' tableidx?
    | 'table.grow' tableidx?
    | 'table.fill' tableidx?
    | 'table.copy' (tableidx tableidx)?
    | 'table.init' tableidx? elemidx
    | 'elem.drop' elemidx
    ;

memarg : Offset? Align?;

Offset : 'offset=' Integer;
Align : 'align=' Integer;

memoryinstr : 'memory.size' memidx?
    | 'memory.grow' memidx?
    | 'memory.fill' memidx?
    | 'memory.copy' (memidx memidx)?
    | 'memory.init' memidx? dataidx
    | 'data.drop' dataidx
    | 'i32.load' memidx? memarg
    | 'i32.load8_s' memidx? memarg
    | 'i32.load8_u' memidx? memarg
    | 'i32.load16_s' memidx? memarg
    | 'i32.load16_u' memidx? memarg
    | 'i64.load' memidx? memarg
    | 'i64.load8_s' memidx? memarg
    | 'i64.load8_u' memidx? memarg
    | 'i64.load16_s' memidx? memarg
    | 'i64.load16_u' memidx? memarg
    | 'i64.load32_s' memidx? memarg
    | 'i64.load32_u' memidx? memarg
    | 'f32.load' memidx? memarg
    | 'f64.load' memidx? memarg
    | 'i32.store' memidx? memarg
    | 'i32.store8' memidx? memarg
    | 'i32.store16' memidx? memarg
    | 'i64.store' memidx? memarg
    | 'i64.store8' memidx? memarg
    | 'i64.store16' memidx? memarg
    | 'i64.store32' memidx? memarg
    | 'f32.store' memidx? memarg
    | 'f64.store' memidx? memarg
    ;

numericinstr : 'i32.const' i32
    | 'i64.const' i64
    | 'f32.const' f32
    | 'f64.const' f64
	| 'i32.eqz'
	| 'i32.eq'
	| 'i32.ne'
	| 'i32.lt_s'
	| 'i32.lt_u'
	| 'i32.gt_s'
	| 'i32.gt_u'
	| 'i32.le_s'
	| 'i32.le_u'
	| 'i32.ge_s'
	| 'i32.ge_u'
	| 'i64.eqz'
	| 'i64.eq'
	| 'i64.ne'
	| 'i64.lt_s'
	| 'i64.lt_u'
	| 'i64.gt_s'
	| 'i64.gt_u'
	| 'i64.le_s'
	| 'i64.le_u'
	| 'i64.ge_s'
	| 'i64.ge_u'
	| 'f32.eq'
	| 'f32.ne'
	| 'f32.lt'
	| 'f32.gt'
	| 'f32.le'
	| 'f32.ge'
	| 'f64.eq'
	| 'f64.ne'
	| 'f64.lt'
	| 'f64.gt'
	| 'f64.le'
	| 'f64.ge'
	| 'i32.clz'
	| 'i32.ctz'
	| 'i32.popcnt'
	| 'i32.add'
	| 'i32.sub'
	| 'i32.mul'
	| 'i32.div_s'
	| 'i32.div_u'
	| 'i32.rem_s'
	| 'i32.rem_u'
	| 'i32.and'
	| 'i32.or'
	| 'i32.xor'
	| 'i32.shl'
	| 'i32.shr_s'
	| 'i32.shr_u'
	| 'i32.rotl'
	| 'i32.rotr'
	| 'i64.clz'
	| 'i64.ctz'
	| 'i64.popcnt'
	| 'i64.add'
	| 'i64.sub'
	| 'i64.mul'
	| 'i64.div_s'
	| 'i64.div_u'
	| 'i64.rem_s'
	| 'i64.rem_u'
	| 'i64.and'
	| 'i64.or'
	| 'i64.xor'
	| 'i64.shl'
	| 'i64.shr_s'
	| 'i64.shr_u'
	| 'i64.rotl'
	| 'i64.rotr'
	| 'f32.abs'
	| 'f32.neg'
	| 'f32.ceil'
	| 'f32.floor'
	| 'f32.trunc'
	| 'f32.nearest'
	| 'f32.sqrt'
	| 'f32.add'
	| 'f32.sub'
	| 'f32.mul'
	| 'f32.div'
	| 'f32.min'
	| 'f32.max'
	| 'f32.copysign'
	| 'f64.abs'
	| 'f64.neg'
	| 'f64.ceil'
	| 'f64.floor'
	| 'f64.trunc'
	| 'f64.nearest'
	| 'f64.sqrt'
	| 'f64.add'
	| 'f64.sub'
	| 'f64.mul'
	| 'f64.div'
	| 'f64.min'
	| 'f64.max'
	| 'f64.copysign'
	| 'i32.wrap_i64'
	| 'i32.trunc_f32_s'
	| 'i32.trunc_f32_u'
	| 'i32.trunc_f64_s'
	| 'i32.trunc_f64_u'
	| 'i64.extend_i32'
	| 'i64.extend_i32'
	| 'i64.trunc_f32_s'
	| 'i64.trunc_f32_u'
	| 'i64.trunc_f64_s'
	| 'i64.trunc_f64_u'
	| 'f32.convert_i32_s'
	| 'f32.convert_i32_u'
	| 'f32.convert_i64_s'
	| 'f32.convert_i64_u'
	| 'f32.demote_f64'
	| 'f64.convert_i32_s'
	| 'f64.convert_i32_u'
	| 'f64.convert_i64_s'
	| 'f64.convert_i64_u'
	| 'f64.promote_f32'
	| 'i32.reinterpret_f32'
	| 'i64.reinterpret_f64'
	| 'f32.reinterpret_i32'
	| 'f64.reinterpret_i64'
	| 'i32.extend8_s'
	| 'i32.extend16_s'
	| 'i64.extend8_s'
	| 'i64.extend16_s'
	| 'i64.extend32_s'
	| 'i32.trunc_sat_f32_s'
	| 'i32.trunc_sat_f32_u'
	| 'i32.trunc_sat_f64_s'
	| 'i32.trunc_sat_f64_u'
	| 'i64.trunc_sat_f32_s'
	| 'i64.trunc_sat_f32_u'
	| 'i64.trunc_sat_f64_s'
	| 'i64.trunc_sat_f64_u'
    ;
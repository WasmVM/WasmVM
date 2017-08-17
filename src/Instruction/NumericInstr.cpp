#include <NumericInstr.h>

void Instruction::i32_const(Stack &coreStack, std::int32_t val){
	coreStack.push(Value(val));
}
void Instruction::i64_const(Stack &coreStack, std::int64_t val){
	coreStack.push(Value(val));
}
void Instruction::f32_const(Stack &coreStack, float val){
	coreStack.push(Value(val));
}
void Instruction::f64_const(Stack &coreStack, double val){
	coreStack.push(Value(val));
}
void Instruction::i32_eqz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.eqz] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.eqz] Operand type is not i32.", coreStack);
	}
	// eqz
	if(operand->data.i32){
		coreStack.push(Value((std::int32_t) 0));
	}else{
		coreStack.push(Value((std::int32_t) 1));
	}
	delete operand;
}
void Instruction::i32_eq(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.eq] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.eq] Operand type is not i32.", coreStack);
	}
	// eq
	if(operand1->data.i32 == operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_ne(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.ne] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.ne] Operand type is not i32.", coreStack);
	}
	// ne
	if(operand1->data.i32 != operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_lt_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.lt_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.lt_s] Operand type is not i32.", coreStack);
	}
	// lt
	if(operand1->data.i32 < operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_lt_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.lt_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.lt_u] Operand type is not i32.", coreStack);
	}
	// lt
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand1->data.i32 < operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_gt_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.gt_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.gt_s] Operand type is not i32.", coreStack);
	}
	// gt
	if(operand1->data.i32 > operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_gt_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.gt_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.gt_u] Operand type is not i32.", coreStack);
	}
	// gt
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand1->data.i32 > operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_le_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.le_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.le_s] Operand type is not i32.", coreStack);
	}
	// le
	if(operand1->data.i32 <= operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_le_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.le_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.le_u] Operand type is not i32.", coreStack);
	}
	// le
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand1->data.i32 <= operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_ge_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.ge_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.ge_s] Operand type is not i32.", coreStack);
	}
	// ge
	if(operand1->data.i32 >= operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i32_ge_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.ge_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.ge_u] Operand type is not i32.", coreStack);
	}
	// ge
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand1->data.i32 >= operand2->data.i32){
		coreStack.push(Value((std::int32_t) 1));
	}else{
		coreStack.push(Value((std::int32_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_eqz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.eqz] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.eqz] Operand type is not i64.", coreStack);
	}
	// eqz
	if(operand->data.i64){
		coreStack.push(Value((std::int64_t) 0));
	}else{
		coreStack.push(Value((std::int64_t) 1));
	}
	delete operand;
}
void Instruction::i64_eq(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.eq] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.eq] Operand type is not i64.", coreStack);
	}
	// eq
	if(operand1->data.i64 == operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_ne(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.ne] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.ne] Operand type is not i64.", coreStack);
	}
	// ne
	if(operand1->data.i64 != operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_lt_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.lt_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.lt_s] Operand type is not i64.", coreStack);
	}
	// lt
	if(operand1->data.i64 < operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_lt_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.lt_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.lt_u] Operand type is not i64.", coreStack);
	}
	// lt
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand1->data.i64 < operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_gt_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.gt_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.gt_s] Operand type is not i64.", coreStack);
	}
	// gt
	if(operand1->data.i64 > operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_gt_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.gt_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.gt_u] Operand type is not i64.", coreStack);
	}
	// gt
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand1->data.i64 > operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_le_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.le_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.le_s] Operand type is not i64.", coreStack);
	}
	// le
	if(operand1->data.i64 <= operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_le_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.le_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.le_u] Operand type is not i64.", coreStack);
	}
	// le
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand1->data.i64 <= operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_ge_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.ge_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.ge_s] Operand type is not i64.", coreStack);
	}
	// ge
	if(operand1->data.i64 >= operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::i64_ge_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.ge_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.ge_u] Operand type is not i64.", coreStack);
	}
	// ge
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand1->data.i64 >= operand2->data.i64){
		coreStack.push(Value((std::int64_t) 1));
	}else{
		coreStack.push(Value((std::int64_t) 0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_eq(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.eq] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.eq] Operand type is not f32.", coreStack);
	}
	// eq
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 1.0));
	}else if(operand1->data.f32 == operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_ne(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.ne] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.ne] Operand type is not f32.", coreStack);
	}
	// ne
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(operand1->data.f32 != operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_lt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.lt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.lt] Operand type is not f32.", coreStack);
	}
	// lt
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(Value((float) 0.0));
		}else{
			coreStack.push(Value((float) 1.0));
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(Value((float) 1.0));
		}else{
			coreStack.push(Value((float) 0.0));
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(operand1->data.f32 < operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_gt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.gt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.gt] Operand type is not f32.", coreStack);
	}
	// gt
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(Value((float) 1.0));
		}else{
			coreStack.push(Value((float) 0.0));
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(Value((float) 0.0));
		}else{
			coreStack.push(Value((float) 1.0));
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(operand1->data.f32 > operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_le(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.le] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.le] Operand type is not f32.", coreStack);
	}
	// le
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(Value((float) 0.0));
		}else{
			coreStack.push(Value((float) 1.0));
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(Value((float) 1.0));
		}else{
			coreStack.push(Value((float) 0.0));
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 1.0));
	}else if(operand1->data.f32 <= operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f32_ge(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.ge] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.ge] Operand type is not f32.", coreStack);
	}
	// ge
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		coreStack.push(Value((float) 0.0));
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(Value((float) 1.0));
		}else{
			coreStack.push(Value((float) 0.0));
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(Value((float) 0.0));
		}else{
			coreStack.push(Value((float) 1.0));
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand1->data.f32)){
		coreStack.push(Value((float) 1.0));
	}else if(operand1->data.f32 >= operand2->data.f32){
		coreStack.push(Value((float) 1.0));
	}else{
		coreStack.push(Value((float) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_eq(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.eq] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.eq] Operand type is not f64.", coreStack);
	}
	// eq
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 1.0));
	}else if(operand1->data.f64 == operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_ne(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.ne] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.ne] Operand type is not f64.", coreStack);
	}
	// ne
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(operand1->data.f64 != operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_lt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.lt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.lt] Operand type is not f64.", coreStack);
	}
	// lt
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(Value((double) 0.0));
		}else{
			coreStack.push(Value((double) 1.0));
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(Value((double) 1.0));
		}else{
			coreStack.push(Value((double) 0.0));
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(operand1->data.f64 < operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_gt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.gt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.gt] Operand type is not f64.", coreStack);
	}
	// gt
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(Value((double) 1.0));
		}else{
			coreStack.push(Value((double) 0.0));
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(Value((double) 0.0));
		}else{
			coreStack.push(Value((double) 1.0));
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(operand1->data.f64 > operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_le(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.le] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.le] Operand type is not f64.", coreStack);
	}
	// le
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(Value((double) 0.0));
		}else{
			coreStack.push(Value((double) 1.0));
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(Value((double) 1.0));
		}else{
			coreStack.push(Value((double) 0.0));
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 1.0));
	}else if(operand1->data.f64 <= operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
void Instruction::f64_ge(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.ge] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.ge] Operand type is not f64.", coreStack);
	}
	// ge
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		coreStack.push(Value((double) 0.0));
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(Value((double) 1.0));
		}else{
			coreStack.push(Value((double) 0.0));
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(Value((double) 0.0));
		}else{
			coreStack.push(Value((double) 1.0));
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand1->data.f64)){
		coreStack.push(Value((double) 1.0));
	}else if(operand1->data.f64 >= operand2->data.f64){
		coreStack.push(Value((double) 1.0));
	}else{
		coreStack.push(Value((double) 0.0));
	}
	delete operand1;
	delete operand2;
}
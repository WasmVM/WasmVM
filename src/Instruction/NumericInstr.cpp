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
void Instruction::i32_clz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.clz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.clz] Operand type is not i32.", coreStack);
	}
	// clz
	if (operand->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 32));
	}else{
		std::int32_t n = 0;
		if (!(operand->data.i32 & 0xFFFF0000)) { n += 16; operand->data.i32 <<= 16; }
		if (!(operand->data.i32 & 0xFF000000)) { n += 8; operand->data.i32 <<= 8; }
		if (!(operand->data.i32 & 0xF0000000)) { n += 4; operand->data.i32 <<= 4; }
		if (!(operand->data.i32 & 0xC0000000)) { n += 2; operand->data.i32 <<= 2; }
		if (!(operand->data.i32 & 0x80000000)) { n += 1; operand->data.i32 <<= 1; }
		coreStack.push(Value(n));
	}
	// clean
	delete operand;
}
void Instruction::i32_ctz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.ctz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.ctz] Operand type is not i32.", coreStack);
	}
	// ctz
	if (operand->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 32));
	}else{
		std::int32_t n = 0;
		if (!(operand->data.i32 & 0x0000FFFF)) { n += 16; operand->data.i32 >>= 16; }
		if (!(operand->data.i32 & 0x000000FF)) { n += 8; operand->data.i32 >>= 8; }
		if (!(operand->data.i32 & 0x0000000F)) { n += 4; operand->data.i32 >>= 4; }
		if (!(operand->data.i32 & 0x00000003)) { n += 2; operand->data.i32 >>= 2; }
		if (!(operand->data.i32 & 0x00000001)) { n += 1; operand->data.i32 >>= 1; }
		coreStack.push(Value(n));
	}
	// clean
	delete operand;
}
void Instruction::i32_popcnt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.ctz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.ctz] Operand type is not i32.", coreStack);
	}
	// popcnt
	operand->data.i32 = (operand->data.i32 & 0x55555555) + ((operand->data.i32 >> 1) & 0x55555555);
	operand->data.i32 = (operand->data.i32 & 0x33333333) + ((operand->data.i32 >> 2) & 0x33333333);
	operand->data.i32 = (operand->data.i32 & 0x0F0F0F0F) + ((operand->data.i32 >> 4) & 0x0F0F0F0F);
	operand->data.i32 = (operand->data.i32 & 0x00FF00FF) + ((operand->data.i32 >> 8) & 0x00FF00FF);
	operand->data.i32 = (operand->data.i32 & 0x0000FFFF) + ((operand->data.i32 >>16) & 0x0000FFFF);
	coreStack.push(*operand);
	// clean
	delete operand;
}
void Instruction::i32_add(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.add] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.add] Operand type is not i32.", coreStack);
	}
	// add
	coreStack.push(Value(operand1->data.i32 + operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_sub(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.sub] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.sub] Operand type is not i32.", coreStack);
	}
	// sub
	coreStack.push(Value(operand1->data.i32 - operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_mul(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.mul] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.mul] Operand type is not i32.", coreStack);
	}
	// mul
	coreStack.push(Value(operand1->data.i32 * operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_div_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.div_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.div_s] Operand type is not i32.", coreStack);
	}
	// div_s
	if(operand2->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i32 / operand2->data.i32));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_div_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.div_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.div_u] Operand type is not i32.", coreStack);
	}
	// div_u
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand2->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i32 / operand2->data.i32));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_rem_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.rem_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.rem_s] Operand type is not i32.", coreStack);
	}
	// rem_s
	if(operand2->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i32 % operand2->data.i32));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_rem_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.rem_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.rem_u] Operand type is not i32.", coreStack);
	}
	// rem_u
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	if(operand2->data.i32 == 0){
		coreStack.push(Value((std::int32_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i32 % operand2->data.i32));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_and(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.and] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.and] Operand type is not i32.", coreStack);
	}
	// and
	coreStack.push(Value(operand1->data.i32 & operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_or(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.or] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.or] Operand type is not i32.", coreStack);
	}
	// or
	coreStack.push(Value(operand1->data.i32 | operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_xor(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.xor] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.xor] Operand type is not i32.", coreStack);
	}
	// xor
	coreStack.push(Value(operand1->data.i32 ^ operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_shl(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.shl] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.shl] Operand type is not i32.", coreStack);
	}
	// shl
	operand2->data.i32 %= 32;
	coreStack.push(Value(operand1->data.i32 << operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_shr_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.shr_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.shr_s] Operand type is not i32.", coreStack);
	}
	// shr_s
	operand2->data.i32 %= 32;
	coreStack.push(Value(operand1->data.i32 >> operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_shr_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.shr_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.shr_u] Operand type is not i32.", coreStack);
	}
	// shr_u
	operand2->data.i32 %= 32;
	operand1->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	operand2->data.i32 *= (-2) * (operand1->data.i32 < 0) + 1;
	coreStack.push(Value(operand1->data.i32 >> operand2->data.i32));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_rotl(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.rotl] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.rotl] Operand type is not i32.", coreStack);
	}
	// rotl
	operand2->data.i32 %= 32; 
	std::uint32_t val = operand1->data.i32;
	coreStack.push((std::int32_t)((val << operand2->data.i32) | (val >> (32 - operand2->data.i32))));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_rotr(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i32.rotr] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i32 || operand2->type != i32){
		throw Exception("[i32.rotr] Operand type is not i32.", coreStack);
	}
	// rotr
	operand2->data.i32 %= 32; 
	std::uint32_t val = operand1->data.i32;
	coreStack.push((std::int32_t)((val >> operand2->data.i32) | (val << (32 - operand2->data.i32))));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_clz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.clz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.clz] Operand type is not i64.", coreStack);
	}
	// clz
	if (operand->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 64));
	}else{
		std::int64_t n = 0;
		if (!(operand->data.i64 & 0xFFFFFFFF00000000)) { n += 32; operand->data.i64 <<= 32; }
		if (!(operand->data.i64 & 0xFFFF000000000000)) { n += 16; operand->data.i64 <<= 16; }
		if (!(operand->data.i64 & 0xFF00000000000000)) { n += 8; operand->data.i64 <<= 8; }
		if (!(operand->data.i64 & 0xF000000000000000)) { n += 4; operand->data.i64 <<= 4; }
		if (!(operand->data.i64 & 0xC000000000000000)) { n += 2; operand->data.i64 <<= 2; }
		if (!(operand->data.i64 & 0x8000000000000000)) { n += 1; operand->data.i64 <<= 1; }
		coreStack.push(Value(n));
	}
	// clean
	delete operand;
}
void Instruction::i64_ctz(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.ctz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.ctz] Operand type is not i64.", coreStack);
	}
	// ctz
	if (operand->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 64));
	}else{
		std::int64_t n = 0;
		if (!(operand->data.i64 & 0x00000000FFFFFFFF)) { n += 32; operand->data.i64 >>= 32; }
		if (!(operand->data.i64 & 0x000000000000FFFF)) { n += 16; operand->data.i64 >>= 16; }
		if (!(operand->data.i64 & 0x00000000000000FF)) { n += 8; operand->data.i64 >>= 8; }
		if (!(operand->data.i64 & 0x000000000000000F)) { n += 4; operand->data.i64 >>= 4; }
		if (!(operand->data.i64 & 0x0000000000000003)) { n += 2; operand->data.i64 >>= 2; }
		if (!(operand->data.i64 & 0x0000000000000001)) { n += 1; operand->data.i64 >>= 1; }
		coreStack.push(Value(n));
	}
	// clean
	delete operand;
}
void Instruction::i64_popcnt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.ctz] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.ctz] Operand type is not i64.", coreStack);
	}
	// popcnt
	operand->data.i64 = (operand->data.i64 & 0x5555555555555555) + ((operand->data.i64 >> 1) & 0x5555555555555555);
	operand->data.i64 = (operand->data.i64 & 0x3333333333333333) + ((operand->data.i64 >> 2) & 0x3333333333333333);
	operand->data.i64 = (operand->data.i64 & 0x0F0F0F0F0F0F0F0F) + ((operand->data.i64 >> 4) & 0x0F0F0F0F0F0F0F0F);
	operand->data.i64 = (operand->data.i64 & 0x00FF00FF00FF00FF) + ((operand->data.i64 >> 8) & 0x00FF00FF00FF00FF);
	operand->data.i64 = (operand->data.i64 & 0x0000FFFF0000FFFF) + ((operand->data.i64 >>16) & 0x0000FFFF0000FFFF);
	operand->data.i64 = (operand->data.i64 & 0x00000000FFFFFFFF) + ((operand->data.i64 >>32) & 0x00000000FFFFFFFF);
	coreStack.push(*operand);
	// clean
	delete operand;
}
void Instruction::i64_add(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.add] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.add] Operand type is not i64.", coreStack);
	}
	// add
	coreStack.push(Value(operand1->data.i64 + operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_sub(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.sub] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.sub] Operand type is not i64.", coreStack);
	}
	// sub
	coreStack.push(Value(operand1->data.i64 - operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_mul(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.mul] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.mul] Operand type is not i64.", coreStack);
	}
	// mul
	coreStack.push(Value(operand1->data.i64 * operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_div_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.div_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.div_s] Operand type is not i64.", coreStack);
	}
	// div_s
	if(operand2->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i64 / operand2->data.i64));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_div_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.div_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.div_u] Operand type is not i64.", coreStack);
	}
	// div_u
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand2->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i64 / operand2->data.i64));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_rem_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.rem_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.rem_s] Operand type is not i64.", coreStack);
	}
	// rem_s
	if(operand2->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i64 % operand2->data.i64));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_rem_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.rem_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.rem_u] Operand type is not i64.", coreStack);
	}
	// rem_u
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	if(operand2->data.i64 == 0){
		coreStack.push(Value((std::int64_t) 0));
	}else{
		coreStack.push(Value(operand1->data.i64 % operand2->data.i64));
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_and(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.and] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.and] Operand type is not i64.", coreStack);
	}
	// and
	coreStack.push(Value(operand1->data.i64 & operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_or(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.or] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.or] Operand type is not i64.", coreStack);
	}
	// or
	coreStack.push(Value(operand1->data.i64 | operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_xor(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.xor] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.xor] Operand type is not i64.", coreStack);
	}
	// xor
	coreStack.push(Value(operand1->data.i64 ^ operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_shl(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.shl] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.shl] Operand type is not i64.", coreStack);
	}
	// shl
	operand2->data.i64 %= 64;
	coreStack.push(Value(operand1->data.i64 << operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_shr_s(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.shr_s] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.shr_s] Operand type is not i64.", coreStack);
	}
	// shr_s
	operand2->data.i64 %= 64;
	coreStack.push(Value(operand1->data.i64 >> operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_shr_u(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.shr_u] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.shr_u] Operand type is not i64.", coreStack);
	}
	// shr_u
	operand2->data.i64 %= 64;
	operand1->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	operand2->data.i64 *= (-2) * (operand1->data.i64 < 0) + 1;
	coreStack.push(Value(operand1->data.i64 >> operand2->data.i64));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_rotl(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.rotl] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.rotl] Operand type is not i64.", coreStack);
	}
	// rotl
	operand2->data.i64 %= 64; 
	std::uint64_t val = operand1->data.i64;
	coreStack.push((std::int64_t)((val << operand2->data.i64) | (val >> (64 - operand2->data.i64))));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i64_rotr(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[i64.rotr] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != i64 || operand2->type != i64){
		throw Exception("[i64.rotr] Operand type is not i64.", coreStack);
	}
	// rotr
	operand2->data.i64 %= 64; 
	std::uint64_t val = operand1->data.i64;
	coreStack.push((std::int64_t)((val >> operand2->data.i64) | (val << (64 - operand2->data.i64))));
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_abs(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.abs] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.abs] Operand type is not f32.", coreStack);
	}
	// abs
	coreStack.push(Util::isPos(operand->data.f32) ? operand->data.f32 : -operand->data.f32);
	// Clean
	delete operand;
}
void Instruction::f32_neg(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.neg] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.neg] Operand type is not f32.", coreStack);
	}
	// neg
	coreStack.push(-operand->data.f32);
	// Clean
	delete operand;
}
void Instruction::f32_ceil(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.ceil] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.ceil] Operand type is not f32.", coreStack);
	}
	// ceil
	if(Util::isNaN(operand->data.f32) || Util::isInf(operand->data.f32) || Util::isZero(operand->data.f32)){
		coreStack.push(operand->data.f32);
	}else if(operand->data.f32 < 0 && operand->data.f32 > -1){
		coreStack.push((float) -0.0);
	}else{
		std::int32_t intVal = operand->data.f32;
		if(intVal < operand->data.f32){
			intVal += 1;
		}
		coreStack.push((float)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f32_floor(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.floor] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.floor] Operand type is not f32.", coreStack);
	}
	// floor
	if(Util::isNaN(operand->data.f32) || Util::isInf(operand->data.f32) || Util::isZero(operand->data.f32)){
		coreStack.push(operand->data.f32);
	}else if(operand->data.f32 > 0 && operand->data.f32 < 1){
		coreStack.push((float) 0.0);
	}else{
		std::int32_t intVal = operand->data.f32;
		if(intVal > operand->data.f32){
			intVal -= 1;
		}
		coreStack.push((float)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f32_trunc(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.trunc] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.trunc] Operand type is not f32.", coreStack);
	}
	// trunc
	if(Util::isNaN(operand->data.f32) || Util::isInf(operand->data.f32) || Util::isZero(operand->data.f32)){
		coreStack.push(operand->data.f32);
	}else if(operand->data.f32 > 0 && operand->data.f32 < 1){
		coreStack.push((float) 0.0);
	}else if(operand->data.f32 < 0 && operand->data.f32 > -1){
		coreStack.push((float) -0.0);
	}else{
		std::int32_t intVal = operand->data.f32;
		coreStack.push((float)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f32_nearest(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.trunc] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.trunc] Operand type is not f32.", coreStack);
	}
	// trunc
	if(Util::isNaN(operand->data.f32) || Util::isInf(operand->data.f32) || Util::isZero(operand->data.f32)){
		coreStack.push(operand->data.f32);
	}else if(operand->data.f32 > 0 && operand->data.f32 < 0.5){
		coreStack.push((float) 0.0);
	}else if(operand->data.f32 < 0 && operand->data.f32 > -0.5){
		coreStack.push((float) -0.0);
	}else{
		std::int32_t intVal = operand->data.f32;
		float decimal = operand->data.f32 - intVal;
		if(decimal > 0.5 || decimal < -0.5){
			if(operand->data.f32 > 0){
				coreStack.push((float)intVal + 1.0);
			}else{
				coreStack.push((float)intVal - 1.0);
			}
		}else if(decimal == 0.5 || decimal == -0.5){
			if(intVal & 1){
				if(operand->data.f32 > 0){
					coreStack.push((float)intVal + 1.0);
				}else{
					coreStack.push((float)intVal - 1.0);
				}
			}else{
				coreStack.push((float)intVal);
			}
		}else{
			coreStack.push((float)intVal);
		}
	}
	// Clean
	delete operand;
}
void Instruction::f32_sqrt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.sqrt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.sqrt] Operand type is not f32.", coreStack);
	}
	// sqrt
	if(!(Util::isPos(operand->data.f32) || Util::isZero(operand->data.f32) || Util::isNaN(operand->data.f32))){
		std::uint32_t nan = 0x7F800001;
		coreStack.push(*(float *)&nan);
	}else if(Util::isNaN(operand->data.f32) || Util::isZero(operand->data.f32) || Util::isInf(operand->data.f32)){
		coreStack.push(operand->data.f32);
	}else{
		coreStack.push((float)sqrt(operand->data.f32));
	}
	// Clean
	delete operand;
}
void Instruction::f32_add(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.add] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.add] Operand type is not f32.", coreStack);
	}
	// add
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isInf(operand1->data.f32) && Util::isInf(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			std::uint32_t nan = 0x7F800001;
			coreStack.push(*(float *)&nan);
		}
	}else if(Util::isInf(operand1->data.f32)){
		coreStack.push(operand1->data.f32);
	}else if(Util::isInf(operand2->data.f32)){
		coreStack.push(operand2->data.f32);
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push((float) 0.0);
		}
	}else{
		coreStack.push(operand1->data.f32 + operand2->data.f32);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_sub(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.sub] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.sub] Operand type is not f32.", coreStack);
	}
	// sub
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isInf(operand1->data.f32) && Util::isInf(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			std::uint32_t nan = 0x7F800001;
			coreStack.push(*(float *)&nan);
		}else{
			coreStack.push(operand1->data.f32);
		}
	}else if(Util::isInf(operand1->data.f32)){
		coreStack.push(operand1->data.f32);
	}else if(Util::isInf(operand2->data.f32)){
		coreStack.push(-operand2->data.f32);
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			coreStack.push((float) 0.0);
		}else{
			coreStack.push(operand1->data.f32);
		}
	}else{
		coreStack.push(operand1->data.f32 - operand2->data.f32);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_mul(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.mul] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.mul] Operand type is not f32.", coreStack);
	}
	// mul
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if((Util::isInf(operand1->data.f32) && Util::isZero(operand2->data.f32)) || (Util::isZero(operand1->data.f32) && Util::isInf(operand2->data.f32))){
		std::uint32_t nan = 0x7F800001;
		coreStack.push(*(float *)&nan);
	}else if(Util::isInf(operand1->data.f32) || Util::isInf(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			std::uint32_t posInf = 0x7F800000;
			coreStack.push(*(float *)&posInf);
		}else{
			std::uint32_t negInf = 0xFF800000;
			coreStack.push(*(float *)&negInf);
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			coreStack.push((float) 0.0);
		}else{
			coreStack.push((float) -0.0);
		}
	}else{
		coreStack.push(operand1->data.f32 * operand2->data.f32);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_div(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.div] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.div] Operand type is not f32.", coreStack);
	}
	// div
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if((Util::isInf(operand1->data.f32) && Util::isInf(operand2->data.f32)) || (Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32))){
		std::uint32_t nan = 0x7F800001;
		coreStack.push(*(float *)&nan);
	}else if(Util::isInf(operand1->data.f32) || Util::isZero(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			std::uint32_t posInf = 0x7F800000;
			coreStack.push(*(float *)&posInf);
		}else{
			std::uint32_t negInf = 0xFF800000;
			coreStack.push(*(float *)&negInf);
		}
	}else if(Util::isZero(operand1->data.f32) || Util::isInf(operand2->data.f32)){
		if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
			coreStack.push((float) 0.0);
		}else{
			coreStack.push((float) -0.0);
		}
	}else{
		coreStack.push(operand1->data.f32 / operand2->data.f32);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_min(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.min] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.min] Operand type is not f32.", coreStack);
	}
	// min
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(operand2->data.f32);
		}else{
			coreStack.push(operand1->data.f32);
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32) && !Util::eqSign(operand1->data.f32, operand2->data.f32)){
		coreStack.push((float) -0.0);
	}else{
		if(operand1->data.f32 < operand2->data.f32){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_max(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.max] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.max] Operand type is not f32.", coreStack);
	}
	// max
	if(Util::isNaN(operand1->data.f32) || Util::isNaN(operand2->data.f32)){
		if(Util::isNaN(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isInf(operand1->data.f32)){
		if(Util::isPos(operand1->data.f32)){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}else if(Util::isInf(operand2->data.f32)){
		if(Util::isPos(operand2->data.f32)){
			coreStack.push(operand2->data.f32);
		}else{
			coreStack.push(operand1->data.f32);
		}
	}else if(Util::isZero(operand1->data.f32) && Util::isZero(operand2->data.f32) && !Util::eqSign(operand1->data.f32, operand2->data.f32)){
		coreStack.push((float) 0.0);
	}else{
		if(operand1->data.f32 > operand2->data.f32){
			coreStack.push(operand1->data.f32);
		}else{
			coreStack.push(operand2->data.f32);
		}
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f32_copysign(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f32.copysign] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f32 || operand2->type != f32){
		throw Exception("[f32.copysign] Operand type is not f32.", coreStack);
	}
	// copysign
	if(Util::eqSign(operand1->data.f32, operand2->data.f32)){
		coreStack.push(operand1->data.f32);
	}else{
		coreStack.push(-operand1->data.f32);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_abs(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.abs] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.abs] Operand type is not f64.", coreStack);
	}
	// abs
	coreStack.push(Util::isPos(operand->data.f64) ? operand->data.f64 : -operand->data.f64);
	// Clean
	delete operand;
}
void Instruction::f64_neg(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.neg] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.neg] Operand type is not f64.", coreStack);
	}
	// neg
	coreStack.push(-operand->data.f64);
	// Clean
	delete operand;
}
void Instruction::f64_ceil(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.ceil] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.ceil] Operand type is not f64.", coreStack);
	}
	// ceil
	if(Util::isNaN(operand->data.f64) || Util::isInf(operand->data.f64) || Util::isZero(operand->data.f64)){
		coreStack.push(operand->data.f64);
	}else if(operand->data.f64 < 0 && operand->data.f64 > -1){
		coreStack.push((double) -0.0);
	}else{
		std::int64_t intVal = operand->data.f64;
		if(intVal < operand->data.f64){
			intVal += 1;
		}
		coreStack.push((double)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f64_floor(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.floor] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.floor] Operand type is not f64.", coreStack);
	}
	// floor
	if(Util::isNaN(operand->data.f64) || Util::isInf(operand->data.f64) || Util::isZero(operand->data.f64)){
		coreStack.push(operand->data.f64);
	}else if(operand->data.f64 > 0 && operand->data.f64 < 1){
		coreStack.push((double) 0.0);
	}else{
		std::int64_t intVal = operand->data.f64;
		if(intVal > operand->data.f64){
			intVal -= 1;
		}
		coreStack.push((double)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f64_trunc(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.trunc] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.trunc] Operand type is not f64.", coreStack);
	}
	// trunc
	if(Util::isNaN(operand->data.f64) || Util::isInf(operand->data.f64) || Util::isZero(operand->data.f64)){
		coreStack.push(operand->data.f64);
	}else if(operand->data.f64 > 0 && operand->data.f64 < 1){
		coreStack.push((double) 0.0);
	}else if(operand->data.f64 < 0 && operand->data.f64 > -1){
		coreStack.push((double) -0.0);
	}else{
		std::int64_t intVal = operand->data.f64;
		coreStack.push((double)intVal);
	}
	// Clean
	delete operand;
}
void Instruction::f64_nearest(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.trunc] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.trunc] Operand type is not f64.", coreStack);
	}
	// trunc
	if(Util::isNaN(operand->data.f64) || Util::isInf(operand->data.f64) || Util::isZero(operand->data.f64)){
		coreStack.push(operand->data.f64);
	}else if(operand->data.f64 > 0 && operand->data.f64 < 0.5){
		coreStack.push((double) 0.0);
	}else if(operand->data.f64 < 0 && operand->data.f64 > -0.5){
		coreStack.push((double) -0.0);
	}else{
		std::int64_t intVal = operand->data.f64;
		double decimal = operand->data.f64 - intVal;
		if(decimal > 0.5 || decimal < -0.5){
			if(operand->data.f64 > 0){
				coreStack.push((double)intVal + 1.0);
			}else{
				coreStack.push((double)intVal - 1.0);
			}
		}else if(decimal == 0.5 || decimal == -0.5){
			if(intVal & 1){
				if(operand->data.f64 > 0){
					coreStack.push((double)intVal + 1.0);
				}else{
					coreStack.push((double)intVal - 1.0);
				}
			}else{
				coreStack.push((double)intVal);
			}
		}else{
			coreStack.push((double)intVal);
		}
	}
	// Clean
	delete operand;
}
void Instruction::f64_sqrt(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.sqrt] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.sqrt] Operand type is not f64.", coreStack);
	}
	// sqrt
	if(!(Util::isPos(operand->data.f64) || Util::isZero(operand->data.f64) || Util::isNaN(operand->data.f64))){
		std::uint64_t nan = 0x7FF0000000000001;
		coreStack.push(*(double *)&nan);
	}else if(Util::isNaN(operand->data.f64) || Util::isZero(operand->data.f64) || Util::isInf(operand->data.f64)){
		coreStack.push(operand->data.f64);
	}else{
		coreStack.push((double)sqrt(operand->data.f64));
	}
	// Clean
	delete operand;
}
void Instruction::f64_add(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.add] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.add] Operand type is not f64.", coreStack);
	}
	// add
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isInf(operand1->data.f64) && Util::isInf(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			std::uint64_t nan = 0x7FF0000000000001;
			coreStack.push(*(double *)&nan);
		}
	}else if(Util::isInf(operand1->data.f64)){
		coreStack.push(operand1->data.f64);
	}else if(Util::isInf(operand2->data.f64)){
		coreStack.push(operand2->data.f64);
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push((double) 0.0);
		}
	}else{
		coreStack.push(operand1->data.f64 + operand2->data.f64);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_sub(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.sub] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.sub] Operand type is not f64.", coreStack);
	}
	// sub
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isInf(operand1->data.f64) && Util::isInf(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			std::uint64_t nan = 0x7FF0000000000001;
			coreStack.push(*(double *)&nan);
		}else{
			coreStack.push(operand1->data.f64);
		}
	}else if(Util::isInf(operand1->data.f64)){
		coreStack.push(operand1->data.f64);
	}else if(Util::isInf(operand2->data.f64)){
		coreStack.push(-operand2->data.f64);
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			coreStack.push((double) 0.0);
		}else{
			coreStack.push(operand1->data.f64);
		}
	}else{
		coreStack.push(operand1->data.f64 - operand2->data.f64);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_mul(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.mul] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.mul] Operand type is not f64.", coreStack);
	}
	// mul
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if((Util::isInf(operand1->data.f64) && Util::isZero(operand2->data.f64)) || (Util::isZero(operand1->data.f64) && Util::isInf(operand2->data.f64))){
		std::uint64_t nan = 0x7FF0000000000001;
		coreStack.push(*(double *)&nan);
	}else if(Util::isInf(operand1->data.f64) || Util::isInf(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			std::uint64_t posInf = 0x7FF0000000000000;
			coreStack.push(*(double *)&posInf);
		}else{
			std::uint64_t negInf = 0xFFF0000000000000;
			coreStack.push(*(double *)&negInf);
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			coreStack.push((double) 0.0);
		}else{
			coreStack.push((double) -0.0);
		}
	}else{
		coreStack.push(operand1->data.f64 * operand2->data.f64);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_div(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.div] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.div] Operand type is not f64.", coreStack);
	}
	// div
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if((Util::isInf(operand1->data.f64) && Util::isInf(operand2->data.f64)) || (Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64))){
		std::uint64_t nan = 0x7FF0000000000001;
		coreStack.push(*(double *)&nan);
	}else if(Util::isInf(operand1->data.f64) || Util::isZero(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			std::uint64_t posInf = 0x7FF0000000000000;
			coreStack.push(*(double *)&posInf);
		}else{
			std::uint64_t negInf = 0xFFF0000000000000;
			coreStack.push(*(double *)&negInf);
		}
	}else if(Util::isZero(operand1->data.f64) || Util::isInf(operand2->data.f64)){
		if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
			coreStack.push((double) 0.0);
		}else{
			coreStack.push((double) -0.0);
		}
	}else{
		coreStack.push(operand1->data.f64 / operand2->data.f64);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_min(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.min] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.min] Operand type is not f64.", coreStack);
	}
	// min
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(operand2->data.f64);
		}else{
			coreStack.push(operand1->data.f64);
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64) && !Util::eqSign(operand1->data.f64, operand2->data.f64)){
		coreStack.push((double) -0.0);
	}else{
		if(operand1->data.f64 < operand2->data.f64){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_max(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.max] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.max] Operand type is not f64.", coreStack);
	}
	// max
	if(Util::isNaN(operand1->data.f64) || Util::isNaN(operand2->data.f64)){
		if(Util::isNaN(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isInf(operand1->data.f64)){
		if(Util::isPos(operand1->data.f64)){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}else if(Util::isInf(operand2->data.f64)){
		if(Util::isPos(operand2->data.f64)){
			coreStack.push(operand2->data.f64);
		}else{
			coreStack.push(operand1->data.f64);
		}
	}else if(Util::isZero(operand1->data.f64) && Util::isZero(operand2->data.f64) && !Util::eqSign(operand1->data.f64, operand2->data.f64)){
		coreStack.push((double) 0.0);
	}else{
		if(operand1->data.f64 > operand2->data.f64){
			coreStack.push(operand1->data.f64);
		}else{
			coreStack.push(operand2->data.f64);
		}
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::f64_copysign(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 2){
		throw Exception("[f64.copysign] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != f64 || operand2->type != f64){
		throw Exception("[f64.copysign] Operand type is not f64.", coreStack);
	}
	// copysign
	if(Util::eqSign(operand1->data.f64, operand2->data.f64)){
		coreStack.push(operand1->data.f64);
	}else{
		coreStack.push(-operand1->data.f64);
	}
	// Clean
	delete operand1;
	delete operand2;
}
void Instruction::i32_wrap_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.wrap/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i32.wrap/i64] Operand type is not i64.", coreStack);
	}
	// wrap
	coreStack.push((std::int32_t)operand->data.i64);
	// Clean
	delete operand;
}
void Instruction::i32_trunc_s_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.trunc_s/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[i32.trunc_s/f32] Operand type is not f32.", coreStack);
	}
	// trunc_s
	coreStack.push((std::int32_t)operand->data.f32);
	// Clean
	delete operand;
}
void Instruction::i32_trunc_u_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.trunc_u/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[i32.trunc_u/f32] Operand type is not f32.", coreStack);
	}
	// trunc_u
	std::int32_t val = operand->data.f32;
	val *= (-2) * (val < 0) + 1;
	coreStack.push(val);
	// Clean
	delete operand;
}
void Instruction::i32_trunc_s_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.trunc_s/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[i32.trunc_s/f64] Operand type is not f64.", coreStack);
	}
	// trunc_s
	coreStack.push((std::int32_t)operand->data.f64);
	// Clean
	delete operand;
}
void Instruction::i32_trunc_u_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.trunc_u/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[i32.trunc_u/f64] Operand type is not f64.", coreStack);
	}
	// trunc_u
	std::int32_t val = operand->data.f64;
	val *= (-2) * (val < 0) + 1;
	coreStack.push(val);
	// Clean
	delete operand;
}
void Instruction::i64_extend_s_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.extend_s/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i64.extend_s/i32] Operand type is not i32.", coreStack);
	}
	// extend_s
	coreStack.push((std::int64_t)operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::i64_extend_u_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.extend_u/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i64.extend_u/i32] Operand type is not i32.", coreStack);
	}
	// extend_u
	std::int64_t val = operand->data.i32;
	val *= (-2) * (val < 0) + 1;
	coreStack.push(val);
	// Clean
	delete operand;
}
void Instruction::i64_trunc_s_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.trunc_s/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[i64.trunc_s/f32] Operand type is not f32.", coreStack);
	}
	// trunc_s
	coreStack.push((std::int64_t)operand->data.f32);
	// Clean
	delete operand;
}
void Instruction::i64_trunc_u_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.trunc_u/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[i64.trunc_u/f32] Operand type is not f32.", coreStack);
	}
	// trunc_u
	std::int64_t val = operand->data.f32;
	val *= (-2) * (val < 0) + 1;
	coreStack.push(val);;
	// Clean
	delete operand;
}
void Instruction::i64_trunc_s_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.trunc_s/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[i64.trunc_s/f64] Operand type is not f64.", coreStack);
	}
	// trunc_s
	coreStack.push((std::int64_t)operand->data.f64);
	// Clean
	delete operand;
}
void Instruction::i64_trunc_u_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.trunc_u/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[i64.trunc_u/f64] Operand type is not f64.", coreStack);
	}
	// trunc_u
	std::int64_t val = operand->data.f64;
	val *= (-2) * (val < 0) + 1;
	coreStack.push(val);
	// Clean
	delete operand;
}
void Instruction::f32_convert_s_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.convert_s/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[f32.convert_s/i32] Operand type is not i32.", coreStack);
	}
	// convert_s
	coreStack.push((float)operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::f32_convert_u_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.convert_u/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[f32.convert_u/i32] Operand type is not i32.", coreStack);
	}
	// convert_u
	operand->data.i32 *= (-2) * (operand->data.i32 < 0) + 1;
	coreStack.push((float)operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::f32_convert_s_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.convert_s/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[f32.convert_s/i64] Operand type is not i64.", coreStack);
	}
	// convert_s
	coreStack.push((float)operand->data.i64);
	// Clean
	delete operand;
}
void Instruction::f32_convert_u_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.convert_u/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[f32.convert_u/i64] Operand type is not i64.", coreStack);
	}
	// convert_u
	operand->data.i64 *= (-2) * (operand->data.i64 < 0) + 1;
	coreStack.push((float)operand->data.i64);
	// Clean
	delete operand;
}
void Instruction::f32_demote_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.demote/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f32.demote/f64] Operand type is not f64.", coreStack);
	}
	// demote
	if(Util::isNaN(operand->data.f64)){
		if(Util::isPos(operand->data.f64)){
			std::uint32_t posNaN = 0x7F800001;
			coreStack.push(*(float *)&posNaN);
		}else{
			std::uint32_t negNaN = 0xFF800001;
			coreStack.push(*(float *)&negNaN);
		}
	}else if(Util::isInf(operand->data.f64)){
		if(Util::isPos(operand->data.f64)){
			std::uint32_t posInf = 0x7F800000;
			coreStack.push(*(float *)&posInf);
		}else{
			std::uint32_t negInf = 0xFF800000;
			coreStack.push(*(float *)&negInf);
		}
	}else if(Util::isZero(operand->data.f64)){
		if(Util::isPos(operand->data.f64)){
			coreStack.push((float) 0.0);
		}else{
			coreStack.push((float) -0.0);
		}
	}else{
		coreStack.push((float) operand->data.f64);
	}
	// Clean
	delete operand;
}
void Instruction::f64_convert_s_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.convert_s/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[f64.convert_s/i32] Operand type is not i32.", coreStack);
	}
	// convert_s
	coreStack.push((double)operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::f64_convert_u_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.convert_u/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[f64.convert_u/i32] Operand type is not i32.", coreStack);
	}
	// convert_u
	operand->data.i32 *= (-2) * (operand->data.i32 < 0) + 1;
	coreStack.push((double)operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::f64_convert_s_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.convert_s/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[f64.convert_s/i64] Operand type is not i64.", coreStack);
	}
	// convert_s
	coreStack.push((double)operand->data.i64);
	// Clean
	delete operand;
}
void Instruction::f64_convert_u_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.convert_u/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[f64.convert_u/i64] Operand type is not i64.", coreStack);
	}
	// convert_u
	operand->data.i64 *= (-2) * (operand->data.i64 < 0) + 1;
	coreStack.push((double)operand->data.i64);
	// Clean
	delete operand;
}
void Instruction::f64_promote_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.promote/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f64.promote/f32] Operand type is not f32.", coreStack);
	}
	// promote
	if(Util::isNaN(operand->data.f32)){
		if(Util::isPos(operand->data.f32)){
			std::uint64_t posNaN = 0x7FF0000000000001;
			coreStack.push(*(double *)&posNaN);
		}else{
			std::uint64_t negNaN = 0xFFF0000000000001;
			coreStack.push(*(double *)&negNaN);
		}
	}else{
		coreStack.push((double) operand->data.f32);
	}
	// Clean
	delete operand;
}
void Instruction::i32_reinterpret_f32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i32.reinterpret/f32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[i32.reinterpret/f32] Operand type is not f32.", coreStack);
	}
	// reinterpret
	coreStack.push(*(std::int32_t *)&operand->data.f32);
	// Clean
	delete operand;
}
void Instruction::i64_reinterpret_f64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[i64.reinterpret/f64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[i64.reinterpret/f64] Operand type is not f64.", coreStack);
	}
	// reinterpret
	coreStack.push(*(std::int64_t *)&operand->data.f64);
	// Clean
	delete operand;
}
void Instruction::f32_reinterpret_i32(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f32.reinterpret/i32] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[f32.reinterpret/i32] Operand type is not i32.", coreStack);
	}
	// reinterpret
	coreStack.push(*(float *)&operand->data.i32);
	// Clean
	delete operand;
}
void Instruction::f64_reinterpret_i64(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[f64.reinterpret/i64] No enough value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[f64.reinterpret/i64] Operand type is not i64.", coreStack);
	}
	// reinterpret
	coreStack.push(*(double *)&operand->data.i64);
	// Clean
	delete operand;
}
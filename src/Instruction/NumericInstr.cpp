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
#include <ParametricInstr.h>
void Instruction::drop(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[drop] No value in the stack.", coreStack);
	}
	// Pop operand
	delete (Value *)coreStack.pop().data;
}
void Instruction::select(Stack &coreStack){
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[select] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[select] Operand type is not i32.", coreStack);
	}
	// Check value operand
	if(coreStack.valueCount() < 2){
		throw Exception("[select] No enough selectable values in the stack.", coreStack);
	}
	// Pop value operands
	Value *operand2 = (Value *)coreStack.pop().data;
	Value *operand1 = (Value *)coreStack.pop().data;
	if(operand1->type != operand2->type){
		throw Exception("[select] Selectable values must be the same type.", coreStack);
	}
	// select
	if(operand->data.i32){
		coreStack.push(*operand1);
	}else{
		coreStack.push(*operand2);
	}
	// clean
	delete operand;
	delete operand1;
	delete operand2;
}
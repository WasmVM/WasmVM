/*
Copyright Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

#include <ParametricInstr.h>

void Instruction::drop(Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[drop] No value in the stack.", coreStack);
	}
	// Pop operand
	delete (Value *)coreStack.pop().data;
}
void Instruction::select(Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[select] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[select] Operand type is not i32.", coreStack);
	}
	// Check value operand
	if(coreStack.valueNum < 2){
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
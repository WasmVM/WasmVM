#include <ControlInstr.h>

void Instruction::invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack, ModuleInst *moduleInst){
	if(funcAddr >= store.funcs.size()){
		throw Exception("[invoke] Function address not exist.", coreStack, moduleInst);
	}
	FuncInst *funcInst = store.funcs.at(funcAddr);
	if(coreStack.valueCount() < funcInst->type.paramTypes.size()){
		throw Exception("[invoke] No enough values for function parameters", coreStack, moduleInst);
	}
	Frame newFrame;
	newFrame.moduleInst = funcInst->module;
	newFrame.locals.resize(funcInst->type.paramTypes.size() + funcInst->code.localTypes.size());
	// Set params
	for(size_t i = 0; i < funcInst->type.paramTypes.size(); ++i){
		Value *val = (Value *)coreStack.pop().data;
		newFrame.locals.at(funcInst->type.paramTypes.size() - 1 - i) = *val;
		delete val;
	}
	// Set locals
	for(size_t i = 0, j = funcInst->type.paramTypes.size(); i < funcInst->code.localTypes.size(); ++i, ++j){
		newFrame.locals.at(j).type = funcInst->code.localTypes.at(i);
		newFrame.locals.at(j).data.i64 = 0;
	}
	// Push frame to stack
	coreStack.push(newFrame);
	// Set label
	Label newLabel;
	newLabel.funcIdx = funcAddr;
	newLabel.contOffset = funcInst->code.body.size() - 1;
	newLabel.resultTypes = funcInst->type.resultTypes;
	// Push label to stack
	coreStack.push(newLabel);
}

void Instruction::ctrl_end(Stack &coreStack){
	// Check result count
	size_t resCount = coreStack.curLabel->resultTypes.size();
	if(coreStack.valueCount() != resCount){
		if(coreStack.valueCount() > resCount){
			throw Exception("[end] Too many values left in the stack while block ending.", coreStack);
		}else{
			throw Exception("[end] Too few values left in the stack for the block result.", coreStack);
		}
	}
	// Pop values
	std::vector<Value> resultValues;
	for(size_t i = 0; i < resCount; ++i){
		Value *val = (Value *)coreStack.pop().data;
		if(val->type != coreStack.curLabel->resultTypes.at(i)){
			throw Exception("[end] Result types not match.", coreStack);
		}
		resultValues.push_back(*val);
		delete val;
	}
	// Pop label
	StackElem popElem = coreStack.pop();
	if(popElem.type != StackElemType::label){
		throw Exception("[end] There must be a label on the top of stack after popping values.", coreStack);
	}
	// Return from function
	if(coreStack.top().type == StackElemType::frame){
		delete (Frame *)coreStack.pop().data;
	}else{
		coreStack.curLabel->instrOffset = ((Label *)popElem.data)->contOffset;
	}
	delete (Label *)popElem.data;
	// Push values
	for(size_t i = 0; i < resCount; ++i){
		coreStack.push(resultValues.back());
		resultValues.pop_back();
	}
}

void Instruction::ctrl_block(std::vector<char> &funcBody, Stack &coreStack, char blocktype){
	Label newLabel;
	switch(blocktype){
		case TYPE_i32:
			newLabel.resultTypes.push_back(i32);
		break;
		case TYPE_i64:
			newLabel.resultTypes.push_back(i64);
		break;
		case TYPE_f32:
			newLabel.resultTypes.push_back(f32);
		break;
		case TYPE_f64:
			newLabel.resultTypes.push_back(f64);
		break;
		default:
		break;
	}
	newLabel.funcIdx = coreStack.curLabel->funcIdx;
	newLabel.instrOffset = coreStack.curLabel->instrOffset;
	try{
		newLabel.contOffset = Util::getContinueOffset(funcBody, coreStack, newLabel.instrOffset) + 1;
	}catch(const char *e){
		throw Exception(std::string("[block] ") + e, coreStack);
	}
	coreStack.push(newLabel);
}

void Instruction::ctrl_loop(Stack &coreStack, char blocktype){
	Label newLabel;
	switch(blocktype){
		case TYPE_i32:
			newLabel.resultTypes.push_back(i32);
		break;
		case TYPE_i64:
			newLabel.resultTypes.push_back(i64);
		break;
		case TYPE_f32:
			newLabel.resultTypes.push_back(f32);
		break;
		case TYPE_f64:
			newLabel.resultTypes.push_back(f64);
		break;
		default:
		break;
	}
	newLabel.funcIdx = coreStack.curLabel->funcIdx;
	newLabel.instrOffset = coreStack.curLabel->instrOffset;
	newLabel.contOffset = newLabel.instrOffset - 2;
	coreStack.push(newLabel);
}
void Instruction::ctrl_return(Stack &coreStack){
	// Check result count
	size_t resCount = coreStack.curLabel->resultTypes.size();
	if(coreStack.valueCount() < resCount){
		throw Exception("[return] Too few values left in the stack for the block result.", coreStack);
	}
	// Pop values
	std::vector<Value> resultValues;
	for(size_t i = 0; i < resCount; ++i){
		Value *val = (Value *)coreStack.pop().data;
		if(val->type != coreStack.curLabel->resultTypes.at(i)){
			throw Exception("[return] Result types not match.", coreStack);
		}
		resultValues.push_back(*val);
		delete val;
	}
	// Check frame
	if(coreStack.curFrame == nullptr){
		throw Exception("[return] There is no frame to return.", coreStack);
	}
	// Pop unnecessary values, labels and a frame
	StackElem popElem;
	do{
		popElem = coreStack.pop();
		switch(popElem.type){
			case value:
				delete (Value *)popElem.data;
			break;
			case label:
				delete (Label *)popElem.data;				
			break;
			case frame:
				delete (Frame *)popElem.data;
			break;
		}
	}while(popElem.type != frame);
	// Push values
	for(size_t i = 0; i < resCount; ++i){
		coreStack.push(resultValues.back());
		resultValues.pop_back();
	}
}
void Instruction::ctrl_call(std::uint32_t funcAddr, Store &store, Stack &coreStack){
	// Check function index exist
	if(funcAddr >= coreStack.curFrame->moduleInst->funcaddrs.size()){
		throw Exception("[call] Function index not exist.", coreStack);
	}
	// Invoke
	invoke(coreStack.curFrame->moduleInst->funcaddrs.at(funcAddr), store, coreStack);
}
void Instruction::ctrl_call_indirect(std::uint32_t typeidx, Store &store, Stack &coreStack){
	// Bypass reserved
	coreStack.curLabel->instrOffset += 1;
	// Check table exist
	if(coreStack.curFrame->moduleInst->tableaddrs.size() <= 0){
		throw Exception("[call_indirect] No table in this module.", coreStack);
	}
	std::uint32_t tableAddr = coreStack.curFrame->moduleInst->tableaddrs.at(0);
	if(tableAddr >= store.tables.size()){
		throw Exception("[call_indirect] Table address not exist in the store.", coreStack);
	}
	// Check function type
	if(typeidx >= coreStack.curFrame->moduleInst->types.size()){
		throw Exception("[call_indirect] Function type not exist in this module.", coreStack);
	}
	// Check operand
	if(coreStack.valueCount() < 1){
		throw Exception("[call_indirect] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[call_indirect] Operand type is not i32.", coreStack);
	}
	if((std::uint32_t)operand->data.i32 >= store.tables.at(0)->elem.size()){
		throw Exception("[call_indirect] Element not exists.", coreStack);
	}
	// Function address
	std::uint32_t funcAddr = store.tables.at(0)->elem.at(operand->data.i32);
	if(funcAddr >= store.funcs.size()){
		throw Exception("[call_indirect] Function not exists.", coreStack);
	}
	// Match type
	if(store.funcs.at(funcAddr)->type != coreStack.curFrame->moduleInst->types.at(typeidx)){
		throw Exception("[call_indirect] Function type not match.", coreStack);
	}
	// Invoke
	invoke(funcAddr, store, coreStack);
}
void Instruction::ctrl_br(std::uint32_t depth, Stack &coreStack){
	// Check label
	unsigned int labelCount = 0;
	std::vector<ValType> resTypes;
	std::uint64_t continueOffset = 0;
	for(std::list<StackElem>::iterator stackIt = coreStack.begin(); stackIt != coreStack.end(); ++stackIt){
		if(stackIt->type == label){
			if(++labelCount >= depth + 1){
				Label *target = (Label *)stackIt->data;
				resTypes = target->resultTypes;
				continueOffset = target->contOffset;
				break;
			}
		}else if(stackIt->type == frame){
			throw Exception("[br] No enough lables to jump.", coreStack);
		}
	}
	if(labelCount < depth + 1){
		throw Exception("[br] No enough lables to jump.", coreStack);
	}
	// Check result count
	if(coreStack.valueCount() < resTypes.size()){
		throw Exception("[br] Too few values left in the stack for target block result.", coreStack);
	}
	// Pop values
	std::vector<Value> resultValues;
	for(size_t i = 0; i < resTypes.size(); ++i){
		Value *val = (Value *)coreStack.pop().data;
		if(val->type != resTypes.at(i)){
			throw Exception("[br] Result types not match.", coreStack);
		}
		resultValues.push_back(*val);
		delete val;
	}
	// Pop Labels
	for(size_t i = 0; i < depth + 1; ++i){
		while(coreStack.top().type == value){
			delete (Value *)coreStack.pop().data;
		}
		if(coreStack.top().type != label){
			throw Exception("[br] Stack top must be label after popping values.", coreStack);
		}else{
			delete (Label *)coreStack.pop().data;
		}
	}
	// Set continue
	if(coreStack.curLabel == nullptr){
		// Pop frame
		delete (Frame *)coreStack.pop().data;
	}else{
		coreStack.curLabel->instrOffset = continueOffset;
	}
	// Push values
	for(size_t i = 0; i < resTypes.size(); ++i){
		coreStack.push(resultValues.back());
		resultValues.pop_back();
	}
}
void Instruction::ctrl_unreachable(Store &store, Stack &coreStack){
#ifdef USE_SYSCALL

#else
#ifdef NDEBUG
	throw Exception("[unreachable] Trap without syscall provided.", coreStack);
#else
	std::cout << "Values in the stack:" << std::endl;
	while(coreStack.valueCount()){
		Value *valPtr = (Value *)coreStack.pop().data;
		switch (valPtr->type){
			case i32:
				std::cout << "Type: i32, Value: " << valPtr->data.i32 << std::endl;
			break;
			case i64:
				std::cout << "Type: i64, Value: " << valPtr->data.i64 << std::endl;
			break;
			case f32:
				std::cout << "Type: f32, Value: " << valPtr->data.f32 << std::endl;
			break;
			case f64:
				std::cout << "Type: f64, Value: " << valPtr->data.f64 << std::endl;
			break;
		}
		delete valPtr;
	}
#endif // NDEBUG
#endif // USE_SYSCALL
}
void Instruction::ctrl_br_table(std::vector<std::uint32_t> &depths, Stack &coreStack){
	// Check stack
	if(coreStack.valueCount() < 1){
		throw Exception("[br_table] No value in the stack.", coreStack);
	}
	// Pop value
	Value *val = (Value *)coreStack.pop().data;
	if(val->type != i32){
		throw Exception("[br_table] Value type is not i32.", coreStack);
	}
	// br
	if(val->data.i32 >= 0 && val->data.i32 < (std::int32_t)depths.size() - 1){
		ctrl_br(depths.at(val->data.i32), coreStack);
	}else{
		ctrl_br(depths.back(), coreStack);
	}
	delete val;
}
void Instruction::ctrl_br_if(std::uint32_t depth, Stack &coreStack){
	// Check stack
	if(coreStack.valueCount() < 1){
		throw Exception("[br_if] No value in the stack.", coreStack);
	}
	// Pop value
	Value *val = (Value *)coreStack.pop().data;
	if(val->type != i32){
		throw Exception("[br_if] Value type is not i32.", coreStack);
	}
	// br
	if(val->data.i32){
		ctrl_br(depth, coreStack);
	}
	delete val;
}
void Instruction::ctrl_if(std::vector<char> &funcBody, Stack &coreStack, char blocktype){
	// Check stack
	if(coreStack.valueCount() < 1){
		throw Exception("[if] No value in the stack.", coreStack);
	}
	// Pop value
	Value *val = (Value *)coreStack.pop().data;
	if(val->type != i32){
		throw Exception("[if] Value type is not i32.", coreStack);
	}
	// Create new label
	Label newLabel;
	switch(blocktype){
		case TYPE_i32:
			newLabel.resultTypes.push_back(i32);
		break;
		case TYPE_i64:
			newLabel.resultTypes.push_back(i64);
		break;
		case TYPE_f32:
			newLabel.resultTypes.push_back(f32);
		break;
		case TYPE_f64:
			newLabel.resultTypes.push_back(f64);
		break;
		default:
		break;
	}
	newLabel.funcIdx = coreStack.curLabel->funcIdx;
	// if
	if(val->data.i32){
		newLabel.instrOffset = coreStack.curLabel->instrOffset;
		try{
			newLabel.contOffset = Util::getContinueOffset(funcBody, coreStack, newLabel.instrOffset) + 1;
		}catch(const char *e){
			throw Exception(std::string("[if] ") + e, coreStack);
		}
	}else{
		try{
			newLabel.instrOffset = Util::getContinueOffset(funcBody, coreStack, coreStack.curLabel->instrOffset, true);
			newLabel.contOffset = Util::getContinueOffset(funcBody, coreStack, newLabel.instrOffset - 1) + 1;
		}catch(const char *e){
			throw Exception(std::string("[if] ") + e, coreStack);
		}
	}
	coreStack.push(newLabel);
	delete val;
}
void Instruction::ctrl_else(Stack &coreStack){
	// Check result count
	size_t resCount = coreStack.curLabel->resultTypes.size();
	if(coreStack.valueCount() != resCount){
		throw Exception("[else] Values left in the stack not the correct number for result.", coreStack);
	}
	// Pop values
	std::vector<Value> resultValues;
	for(size_t i = 0; i < resCount; ++i){
		Value *val = (Value *)coreStack.pop().data;
		if(val->type != coreStack.curLabel->resultTypes.at(i)){
			throw Exception("[else] Result types not match.", coreStack);
		}
		resultValues.push_back(*val);
		delete val;
	}
	// Pop label
	StackElem popElem = coreStack.pop();
	if(popElem.type != StackElemType::label){
		throw Exception("[else] There must be a label on the top of stack after pop result value.", coreStack);
	}
	// Return from function
	coreStack.curLabel->instrOffset = ((Label *)popElem.data)->contOffset;
	delete (Label *)popElem.data;
	// Push values
	for(size_t i = 0; i < resCount; ++i){
		coreStack.push(resultValues.back());
		resultValues.pop_back();
	}
}
#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <core/Stack.h>
#include <core/Store.h>
#include <core/Runtime.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/vector_p.h>
#include <instance/FuncInst.h>
#include <instance/ModuleInst.h>
#include <instance/ControlInstrInst.h>
}
#undef _Bool

SKYPAT_F(Runtime_control_end, valid_single_layer)
{
    Stack stack = new_Stack();
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType type = new_FuncType();
    ValueType result1 = Value_i32;
    vector_push_back(type->results, &result1);
    vector_push_back(module->types, type);
    vector_push_back(store->funcs, new_FuncInst(module, type));

    // Label
    push_Frame(stack, new_Frame(module));
    Label label = new_Label(0, 2, 4);
    label_set_resultTypes(label, type->results);
    push_Label(stack, label);
    push_Value(stack, new_i32Value(4));

    // Check
    EXPECT_EQ(runtime_return(stack, store), 0);
    EXPECT_EQ(stack_cur_label(stack), NULL);
    EXPECT_EQ(stack_cur_frame(stack), NULL);
    Value* result = NULL;
    EXPECT_EQ(pop_Value(stack, &result), 0);
    EXPECT_EQ(result->type, Value_i32);
    EXPECT_EQ(result->value.i32, 4);

    // clean
    free_ModuleInst(module);
    free_Store(store);
    free_Stack(stack);
}

SKYPAT_F(Runtime_control_end, valid_cascated)
{
    Stack stack = new_Stack();
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    vector_push_back(store->funcs, new_FuncInst(module, type));

    // Label
    Frame frame = new_Frame(module);
    push_Frame(stack, frame);
    Label label1 = new_Label(0, 1, 4);
    push_Label(stack, label1);
    Label label2 = new_Label(0, 2, 3);
    label_set_resultTypes(label2, new_vector_p(ValueType, NULL));
    ValueType result1 = Value_i32;
    vector_push_back(label_get_resultTypes(label2), &result1);
    push_Label(stack, label2);
    push_Value(stack, new_i32Value(4));

    // Check
    EXPECT_EQ(runtime_return(stack, store), 0);
    EXPECT_EQ(stack_cur_label(stack), NULL);
    EXPECT_EQ(stack_cur_frame(stack), NULL);

    // clean
    free_ModuleInst(module);
    free_Store(store);
    free_Stack(stack);
}

SKYPAT_F(Runtime_control_end, no_frame)
{
    Stack stack = new_Stack();
    Store store = new_Store();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    vector_push_back(store->funcs, new_FuncInst(module, type));

    // Label
    Label label = new_Label(0, 2, 4);
    label_set_resultTypes(label, type->results);
    push_Label(stack, label);

    // Check
    EXPECT_EQ(runtime_return(stack, store), -1);
    // clean
    free_ModuleInst(module);
    free_Store(store);
    free_Stack(stack);
}
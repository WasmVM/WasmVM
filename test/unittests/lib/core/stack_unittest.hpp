#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <string.h>
#include <stdlib.h>
#include <core/Stack_.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame.h>
#include <dataTypes/Value.h>
#include <instance/ModuleInst.h>
}
#undef _Bool

SKYPAT_F(Stack, create_delete)
{
    // Prepare
    Stack stack = new_Stack();

    // Check
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);

    free_Stack(stack);
}

SKYPAT_F(Stack, push_Label)
{
    // Prepare
    Stack stack = new_Stack();
    push_Label(stack, new_Label(0, 1, 2));

    // Check
    EXPECT_EQ(stack_size(stack->entries), 1);
    EXPECT_NE(stack_head(stack->entries), NULL);
    Label result = stack_top(Label, stack->entries);
    EXPECT_EQ(stack->curLabel, result);
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_instrIndex(result), 1);
    EXPECT_EQ(label_get_contInstr(result), 2);

    free_Stack(stack);
}

SKYPAT_F(Stack, push_Frame)
{
    // Prepare
    Stack stack = new_Stack();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    push_Frame(stack, new_Frame(module));

    // Check
    EXPECT_EQ(stack_size(stack->entries), 1);
    EXPECT_NE(stack_head(stack->entries), NULL);
    Frame result = stack_top(Frame, stack->entries);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(stack->curFrame, result);

    free_Stack(stack);
}

SKYPAT_F(Stack, push_Value)
{
    // Prepare
    Stack stack = new_Stack();
    push_Value(stack, new_i32Value(2));

    // Check
    EXPECT_EQ(stack_size(stack->entries), 1);
    EXPECT_NE(stack_head(stack->entries), NULL);
    Value* result = stack_top(Value*, stack->entries);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(stack->curFrame, NULL);
    EXPECT_EQ(result->value.i32, 2);

    free_Stack(stack);
}

SKYPAT_F(Stack, pop_Label_valid)
{
    // Prepare
    Stack stack = new_Stack();
    push_Label(stack, new_Label(0, 1, 2));

    // Check
    Label result = NULL;
    EXPECT_EQ(pop_Label(stack, &result, 1), 0);
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_instrIndex(result), 1);
    EXPECT_EQ(label_get_contInstr(result), 2);
    free_Label(result);

    result = NULL;
    push_Label(stack, new_Label(0, 5, 6));
    push_Value(stack, new_i32Value(10));
    push_Label(stack, new_Label(0, 3, 4));
    push_Value(stack, new_i32Value(20));

    EXPECT_EQ(pop_Label(stack, &result, 1), 0);
    EXPECT_EQ(stack_size(stack->entries), 2);
    EXPECT_NE(stack_head(stack->entries), NULL);
    EXPECT_EQ(label_get_funcAddr(stack->curLabel), 0);
    EXPECT_EQ(label_get_instrIndex(stack->curLabel), 5);
    EXPECT_EQ(label_get_contInstr(stack->curLabel), 6);
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_instrIndex(result), 3);
    EXPECT_EQ(label_get_contInstr(result), 4);
    free_Label(result);

    EXPECT_EQ(pop_Label(stack, &result, 1), 0);
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(label_get_funcAddr(result), 0);
    EXPECT_EQ(label_get_instrIndex(result), 5);
    EXPECT_EQ(label_get_contInstr(result), 6);

    free_Label(result);
    free_Stack(stack);
}

SKYPAT_F(Stack, pop_Label_not_exist_curLabel)
{
    // Prepare
    Stack stack = new_Stack();

    // Check
    Label result = NULL;
    EXPECT_EQ(pop_Label(stack, &result, 1), -1);

    free_Stack(stack);
}

SKYPAT_F(Stack, pop_Frame_valid)
{
    // Prepare
    Stack stack = new_Stack();
    char* moduleName = (char*) malloc(sizeof(char) * 5);
    strcpy(moduleName, "Test");
    ModuleInst* module = new_ModuleInst(moduleName);
    push_Frame(stack, new_Frame(module));

    // Check
    Frame result = NULL;
    EXPECT_EQ(pop_Frame(stack, &result, NULL), 0);
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(stack->curFrame, NULL);
    free_Frame(result);

    result = NULL;
    Label label1 = new_Label(0, 1, 2);
    Frame frame1 = new_Frame(module);
    push_Frame(stack, frame1);
    push_Label(stack, label1);
    push_Value(stack, new_i32Value(10));
    push_Frame(stack, new_Frame(module));
    push_Label(stack, new_Label(0, 3, 4));
    push_Value(stack, new_i32Value(20));

    EXPECT_EQ(pop_Frame(stack, &result, NULL), 0);
    EXPECT_EQ(stack_size(stack->entries), 3);
    EXPECT_NE(stack_head(stack->entries), NULL);
    EXPECT_EQ(stack->curLabel, label1);
    EXPECT_EQ(stack->curFrame, frame1);
    free_Frame(result);

    EXPECT_EQ(pop_Frame(stack, &result, NULL), 0);
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(stack->curFrame, NULL);
    free_Frame(result);

    free_Stack(stack);
    free_ModuleInst(module);
}

SKYPAT_F(Stack, pop_Frame_not_exist_curFrame)
{
    // Prepare
    Stack stack = new_Stack();

    // Check
    Frame result = NULL;
    EXPECT_EQ(pop_Frame(stack, &result, NULL), -1);

    free_Stack(stack);
}

SKYPAT_F(Stack, pop_Value_valid)
{
    // Prepare
    Stack stack = new_Stack();
    push_Value(stack, new_i32Value(3));

    // Check
    Value* result = NULL;
    EXPECT_EQ(pop_Value(stack, &result), 0);
    EXPECT_EQ(stack_size(stack->entries), 0);
    EXPECT_EQ(stack_head(stack->entries), NULL);
    EXPECT_EQ(stack->curLabel, NULL);
    EXPECT_EQ(stack->curFrame, NULL);
    EXPECT_EQ(result->value.i32, 3);

    free_Value(result);
    free_Stack(stack);
}

SKYPAT_F(Stack, pop_Value_not_exist)
{
    // Prepare
    Stack stack = new_Stack();

    // Check
    Value* result = NULL;
    EXPECT_EQ(pop_Value(stack, &result), -2);
    EXPECT_EQ(result, NULL);

    free_Stack(stack);
}
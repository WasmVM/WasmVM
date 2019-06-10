#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <instance/ModuleInst.h>
#include <string.h>
}
#undef _Bool

SKYPAT_F(ModuleInst, create_delete)
{
    // Prepare
    char* name = (char*) malloc(sizeof(char) * 5);
    strcpy(name, "test");
    ModuleInst* moduleInst = new_ModuleInst(name);

    // Check
    EXPECT_EQ(strcmp(moduleInst->name, "test"), 0);
    EXPECT_EQ(vector_size(moduleInst->types), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->types), NULL);
    EXPECT_EQ(vector_size(moduleInst->funcaddrs), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->funcaddrs), NULL);
    EXPECT_EQ(vector_size(moduleInst->memaddrs), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->memaddrs), NULL);
    EXPECT_EQ(vector_size(moduleInst->tableaddrs), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->tableaddrs), NULL);
    EXPECT_EQ(vector_size(moduleInst->globaladdrs), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->globaladdrs), NULL);
    EXPECT_EQ(vector_size(moduleInst->exports), 0);
    EXPECT_EQ(vector_data(uint8_t*, moduleInst->exports), NULL);

    free_ModuleInst(moduleInst);
}

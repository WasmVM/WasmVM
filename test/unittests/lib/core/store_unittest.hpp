#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <dataTypes/Value.h>
#include <core/Store.h>
}
#undef _Bool

SKYPAT_F(Store, create_delete)
{
    // Prepare
    Store store = new_Store();

    // Check
    EXPECT_EQ(vector_size(store->funcs), 0);
    EXPECT_EQ(vector_data(uint8_t*, store->funcs), NULL);
    EXPECT_EQ(vector_size(store->tables), 0);
    EXPECT_EQ(vector_data(uint8_t*, store->tables), NULL);
    EXPECT_EQ(vector_size(store->mems), 0);
    EXPECT_EQ(vector_data(uint8_t*, store->mems), NULL);
    EXPECT_EQ(vector_size(store->globals), 0);
    EXPECT_EQ(vector_data(uint8_t*, store->globals), NULL);

    free_Store(store);
}

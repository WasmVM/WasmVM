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
    Store* store = new_Store();

    // Check
    EXPECT_EQ(store->funcs->length, 0);
    EXPECT_EQ(store->funcs->data, NULL);
    EXPECT_EQ(store->tables->length, 0);
    EXPECT_EQ(store->tables->data, NULL);
    EXPECT_EQ(store->mems->length, 0);
    EXPECT_EQ(store->mems->data, NULL);
    EXPECT_EQ(store->globals->length, 0);
    EXPECT_EQ(store->globals->data, NULL);

    free_Store(store);
}

#ifndef WASMVM_TEST_HASHMAP_T_DEF
#define WASMVM_TEST_HASHMAP_T_DEF

#include <defines.h>
#include <md5.h>

struct _hashmap;

void _hashmap_set_private(const u32_t key_size, byte_t key[key_size], void* value, struct _hashmap** map);
void* _hashmap_get_private(const u32_t key_size, byte_t key[key_size], const struct _hashmap* map);
void free_hashmap(struct _hashmap* map);

#define hashmap_t(T) struct _hashmap*
#define hashmap_set(T, KSize, K, V, H) { \
    T* valuePtr = malloc_func(sizeof(T)); \
    *valuePtr = V; \
    _hashmap_set_private(KSize, (byte_t*)K, valuePtr, &H); \
}
#define hashmap_get(T, KSize, K, H) \
    ((T*)_hashmap_get_private(KSize, (byte_t*)K, H))

#endif

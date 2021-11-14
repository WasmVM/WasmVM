#ifndef WASMVM_TEST_HASHMAP_T_DEF
#define WASMVM_TEST_HASHMAP_T_DEF

#include <defines.h>
#include <md5.h>

struct _hashmap;

void _hashmap_set_private(const u32_t key_size, byte_t key[key_size], void* valuePtr, struct _hashmap** map);
void _hashmap_get_private(const u32_t key_size, byte_t key[key_size], void** valuePtr, const struct _hashmap* map);
void free_hashmap(struct _hashmap* map);

#define hashmap_t(T) struct _hashmap*
#define hashmap_set(KSize, K, Vp, H) _hashmap_set_private(KSize, (byte_t*)K, (void*)Vp, &H);
#define hashmap_get(KSize, K, Vp, H) _hashmap_get_private(KSize, (byte_t*)K, (void**)&Vp, H);

#endif

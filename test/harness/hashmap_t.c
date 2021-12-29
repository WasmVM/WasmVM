#include <hashmap_t.h>

struct _hashmap {
    u64_t key[2];
    void* data;
    _Bool isRed;
    struct _hashmap *root, *left, *right;
};

#define SWAP(T, A, B) \
    A = (T)((u64_t)A ^ (u64_t)B); \
    B = (T)((u64_t)A ^ (u64_t)B); \
    A = (T)((u64_t)A ^ (u64_t)B);

static void adjust_red(struct _hashmap* node)
{
    if(node->root == NULL) {
        // Root
        node->isRed = 0;
    } else if(node->root->isRed) {
        struct _hashmap* grand = node->root->root;
        // Recolor
        if((grand->left && grand->left->isRed) && (grand->right && grand->right->isRed)) {
            grand->left->isRed = 0;
            grand->right->isRed = 0;
            grand->isRed = 1;
            adjust_red(grand);
        } else if(node->root == grand->left) {
            if(node == node->root->left) {
                // Right rotate
                grand->left = node;
                node->root->left = node->root->right;
                node->root->right = grand->right;
                grand->right = node->root;
            } else {
                // Left - Right rotate
                node->root->right = node->left;
                node->left = node->right;
                node->right = grand->right;
                grand->right = node;
            }
            node->root = grand;
            SWAP(void*, grand->data, grand->right->data);
            SWAP(u64_t, grand->key[0], grand->right->key[0]);
            SWAP(u64_t, grand->key[1], grand->right->key[1]);
        } else {
            if(node == node->root->right) {
                // Left rotate
                grand->right = node;
                node->root->right = node->root->left;
                node->root->left = grand->left;
                grand->left = node->root;
            } else {
                // Right - Left rotate
                node->root->left = node->right;
                node->right = node->left;
                node->left = grand->left;
                grand->left = node;
            }
            node->root = grand;
            SWAP(void*, grand->data, grand->left->data);
            SWAP(u64_t, grand->key[0], grand->left->key[0]);
            SWAP(u64_t, grand->key[1], grand->left->key[1]);
        }
    }
}

void _hashmap_set_private(const u32_t key_size, byte_t key[key_size], void* valuePtr, struct _hashmap** map)
{
    // Get hash
    u64_t md5[2];
    md5_hash(key_size, key, md5);
    // Traverse
    struct _hashmap **previous = map, *cursor = *map, *parent = NULL;
    while(cursor != NULL) {
        if(md5[0] == cursor->key[0]) {
            // Equal key 0
            if(md5[1] == cursor->key[1]) {
                // Same key
                free(cursor->data);
                cursor->data = valuePtr;
                return;
            } else {
                // Differ key 1
                parent = cursor;
                if(md5[1] < cursor->key[1]) {
                    previous = &(cursor->left);
                    cursor = cursor->left;
                } else {
                    previous = &(cursor->right);
                    cursor = cursor->right;
                }
            }
        } else {
            // Differ key 0
            parent = cursor;
            if(md5[0] < cursor->key[0]) {
                previous = &(cursor->left);
                cursor = cursor->left;
            } else {
                previous = &(cursor->right);
                cursor = cursor->right;
            }
        }
    }
    // Allocate node
    struct _hashmap* node = (struct _hashmap*) malloc_func(sizeof(struct _hashmap));
    node->key[0] = md5[0];
    node->key[1] = md5[1];
    node->data = valuePtr;
    node->isRed = 1;
    node->left = NULL;
    node->right = NULL;
    node->root = parent;
    *previous = node;
    adjust_red(node);
}

void _hashmap_get_private(const u32_t key_size, byte_t key[key_size], void** valuePtr, const struct _hashmap* map)
{
    // Empty map
    if(map == NULL) {
        *valuePtr = NULL;
        return;
    }
    // Get hash
    u64_t md5[2];
    md5_hash(key_size, key, md5);
    // Traverse
    const struct _hashmap *cursor = map;
    while(cursor != NULL) {
        if(md5[0] == cursor->key[0]) {
            if(md5[1] == cursor->key[1]) {
                // Matched
                *valuePtr = cursor->data;
                return;
            } else {
                if(md5[1] < cursor->key[1]) {
                    cursor = cursor->left;
                } else {
                    cursor = cursor->right;
                }
            }
        } else {
            if(md5[0] < cursor->key[0]) {
                cursor = cursor->left;
            } else {
                cursor = cursor->right;
            }
        }
    }
    *valuePtr = NULL;
}

void free_hashmap(struct _hashmap* map)
{
    while(map != NULL) {
        if(map->left != NULL) {
            map = map->left;
        } else if(map->right != NULL) {
            map = map->right;
        } else {
            struct _hashmap* cur = map;
            map = map->root;
            free(cur->data);
            free(cur);
        }
    }
}
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stdlib.h>

typedef struct key_val_t key_val_t;

typedef uint64_t (*ht_hash_fn)(const char *key, size_t key_len);

typedef struct {
    key_val_t *entries;
    ht_hash_fn hash_fn;
    uint32_t capacity;
    uint32_t size;
} hash_table_t;

/* Initialize a new hash table with the specified capacity and optional
 * custom hash function. Capacity must be a power-of-two. If `hash_fn` is NULL,
 * the default `FNV-1a` hashing function is used. Returns a pointer to the hash
 * table, otherwise NULL. */
hash_table_t *hash_table_init(uint32_t capacity, ht_hash_fn hash_fn);

/* Free the memory allocated for hash table and it's entries. */
void hash_table_free(hash_table_t *ht);

/* Insert a key/value pair into given hash table. Duplicate keys are updated
 * with the new value appended in a comma-separated list. Returns (1) on
 * successful insertion, otherwise (0). */
int hash_table_insert(hash_table_t *ht, const char *key, const char *value);

/* Return pointer to the value associated with the given key, otherwise NULL. */
char *hash_table_lookup(hash_table_t *ht, const char *key);

/* Remove the key/value pair associated with the given key. Returns (1) on
 * successful deletion, otherwise (0). */
int hash_table_delete(hash_table_t *ht, const char *key);

/* Prints the contents of the hash table to stdout. */
void hash_table_debug_print(const hash_table_t *ht);

#endif  // HASH_TABLE_H

#include "hash_table.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Resize at 70% capacity to prevent excessive collisions.
#define LOAD_FACTOR 0.7

struct key_val_t {
    char *key;
    char *value;
    uint8_t is_deleted; /* Acts as a tombstone marker. Set to (1) when deleted,
                           (0) by default. */
};

// FNV-1a [https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function].
static inline uint64_t fnv_1a_hash(const char *key, size_t key_len) {
    uint64_t hash = 0xcbf29ce484222325; /* FNV offset-bias (64-bit) */

    for (size_t i = 0; i < key_len; ++i) {
        hash ^= (unsigned char)key[i];
        hash *= 0x100000001b3; /* FNV prime (64-bit) */
    }

    return hash;
}

static void key_to_lower(char *dest, const char *key) {
    while (*key) {
        *dest++ = (char)tolower((unsigned char)*key++);
    }
    *dest = '\0';
}

// Resize the given hash table. Returns (1) on successful resize, otherwise (0).
static int hash_table_resize(hash_table_t *ht) {
    key_val_t *local_entries = ht->entries;
    uint32_t local_capacity = ht->capacity;

    // Double capacity to ensure it remains a power-of-two.
    key_val_t *new_entries = calloc(local_capacity * 2, sizeof(*new_entries));
    if (!new_entries) {
        perror("ERROR: hash_table_resize (calloc)");
        return 0;
    }

    ht->entries = new_entries;
    ht->capacity *= 2;

    // Set size to 0 before inserting previous entries.
    ht->size = 0;

    // Insert entries from previous allocation to new allocation, as each key
    // needs to be rehashed to account for new capacity.
    for (size_t i = 0; i < local_capacity; ++i) {
        // Insert entries with non-NULL keys only.
        if (local_entries[i].key) {
            hash_table_insert(ht, local_entries[i].key, local_entries[i].value);

            // Clean up allocations for local key/value pairs.
            free(local_entries[i].key);
            free(local_entries[i].value);
        }
    }

    free(local_entries);
    return 1;
}

int hash_table_init(hash_table_t *ht, uint32_t capacity, ht_hash_fn hash_fn) {
    assert(ht && ((capacity & (capacity - 1)) == 0));

    ht->entries = calloc(capacity, sizeof(*ht->entries));
    if (!ht->entries) {
        perror("ERROR: hash_table_init (calloc)");
        return 0;
    }

    ht->hash_fn = hash_fn ? hash_fn : fnv_1a_hash;
    ht->capacity = capacity;
    ht->size = 0;

    return 1;
}

void hash_table_free(hash_table_t *ht) {
    assert(ht);

    if (ht->entries) {
        if (ht->size > 0) {
            for (size_t i = 0; i < ht->capacity; ++i) {
                free(ht->entries[i].key);
                free(ht->entries[i].value);
            }
        }

        free(ht->entries);
    }
}

int hash_table_insert(hash_table_t *ht, const char *key, const char *value) {
    assert(ht && key && value);

    if (ht->size >= LOAD_FACTOR * ht->capacity) {
        if (hash_table_resize(ht) == 0) {
            return 0;
        }
    }

    size_t key_len = strlen(key);
    size_t val_len = strlen(value);

    char *alloc_key = malloc(key_len + 1);
    if (!alloc_key) {
        perror("ERROR: hash_table_insert (malloc)");
        return 0;
    }

    char *alloc_val = malloc(val_len + 1);
    if (!alloc_val) {
        perror("ERROR: hash_table_insert (malloc)");
        return 0;
    }

    memcpy(alloc_key, key, key_len + 1);
    memcpy(alloc_val, value, val_len + 1);

    // Allocate memory for provided key/value pair.
    key_val_t kv = {.key = alloc_key, .value = alloc_val, .is_deleted = 0};

    // Convert key to lowercase to ensure lookups are case-insensitive
    key_to_lower(alloc_key, key);

    // Ensures index is within range [0, capacity - 1].
    size_t idx = ht->hash_fn(alloc_key, key_len) & (ht->capacity - 1);

#ifdef _DEBUG
    printf("hash_table_insert: computed_idx = %lu\tkey = %s\n", idx, key);
#endif

    for (size_t i = 1; i < ht->capacity && ht->entries[idx].key; ++i) {
        // Duplicate key found - update the value of the key, appending values
        // in comma separated list.
        if (strcmp(ht->entries[idx].key, key) == 0) {
            char *new_value = kv.value;
            char *prev_value = ht->entries[idx].value;

            size_t prev_val_len = strlen(prev_value);

            // +3 for null-terminator byte, comma, and space characters.
            char *append_value = malloc(prev_val_len + val_len + 3);
            if (!append_value) {
                perror("ERROR: hash_table_insert (malloc)");
                return 0;
            }

            memcpy(append_value, prev_value, prev_val_len);
            append_value[prev_val_len] = ',';
            append_value[++prev_val_len] = ' ';
            append_value[++prev_val_len] = '\0';

            memcpy(append_value + prev_val_len, new_value, val_len);
            append_value[prev_val_len + val_len] = '\0';

            // Clean up allocations for previous key/value pair.
            free(ht->entries[idx].key);
            free(prev_value);
            free(new_value);

            kv.value = append_value;

            ht->entries[idx] = kv;
            return 1;
        }

        // Using `open addressing` with `quadratic probing` to handle
        // collisions. This approach uses less memory than external chaining by
        // storing all entries in same array, with the downsides of needing to
        // resize and potential clustering.
        size_t probe_offset = ((i * i) + i);
        // Ensures index properly wraps back to 0.
        idx = (idx + probe_offset) & (ht->capacity - 1);
    }

    // Same preference for initially NULL slots or slots marked as deleted.
    ht->entries[idx] = kv;
    ht->size++;

    return 1;
}

char *hash_table_lookup(hash_table_t *ht, const char *key) {
    assert(ht && key);

    size_t key_len = strlen(key);

    // Convert key to lowercase to ensure lookups are case-insensitive
    char lower_key[key_len + 1];
    key_to_lower(lower_key, key);

    // Ensures index is within range [0, capacity - 1].
    size_t idx = ht->hash_fn(lower_key, key_len) & (ht->capacity - 1);

#ifdef _DEBUG
    printf("hash_table_lookup: computed_idx = %lu\tkey = %s\n", idx, key);
#endif

    // Track of the index of first tombstone marker found.
    size_t first_tomb_idx = SIZE_MAX;

    // Set `i` to 0 so indices can be computed at the beginning of the loop.
    for (size_t i = 0; i < ht->capacity; ++i) {
        size_t probe_offset = ((i * i) + i); /* Quadratic probing */
        // Ensures index properly wraps back to 0.
        idx = (idx + probe_offset) & (ht->capacity - 1);

        // Ensure tombstones are checked before NULL entries to maintain probing
        // sequence.
        if (ht->entries[idx].is_deleted) {
            // Tombstone (deleted) slot found - continue with probing sequence.
            //
            // Without tombstone markers, deleted positions would be treated the
            // same as unoccupied slots. This could cause the probe sequence to
            // terminate prematurely, potentially leading to false negatives
            // when trying to find keys that were previously stored at these
            // positions before being deleted.
            if (first_tomb_idx == SIZE_MAX) {
                // Only track first tombstone marker
                first_tomb_idx = idx;
            }
            continue;
        }

        if (!ht->entries[idx].key) {
            break;
        }

        // Key found
        if (strcmp(ht->entries[idx].key, lower_key) == 0) {
            if (first_tomb_idx == SIZE_MAX) {
                // No tombstone marker found - return value directly.
                return ht->entries[idx].value;
            }

            // By setting the key/value pair to the first tombstone index,
            // subsequent lookups with the given key will not need to traverse
            // the entire probing sequence.
            ht->entries[first_tomb_idx] = ht->entries[idx];

            // Mark key's previous slot as empty
            ht->entries[idx].key = NULL;
            ht->entries[idx].value = NULL;

            return ht->entries[first_tomb_idx].value;
        }
    }

    return NULL;
}

int hash_table_delete(hash_table_t *ht, const char *key) {
    assert(ht && key);

    size_t key_len = strlen(key);

    // Convert key to lowercase to ensure lookups are case-insensitive
    char lower_key[key_len + 1];
    key_to_lower(lower_key, key);

    // Ensures index is within range [0, capacity - 1].
    size_t idx = ht->hash_fn(lower_key, key_len) & (ht->capacity - 1);

#ifdef _DEBUG
    printf("hash_table_delete: computed_idx = %lu\tkey = %s\n", idx, key);
#endif

    // Set `i` to 0 so indices can be computed at the beginning of the loop.
    for (size_t i = 0; i < ht->capacity; ++i) {
        size_t probe_offset = ((i * i) + i); /* Quadratic probing */
        // Ensures index properly wraps back to 0.
        idx = (idx + probe_offset) & (ht->capacity - 1);

        // Ensure tombstones are checked before NULL entries to maintain probing
        // sequence.
        if (ht->entries[idx].is_deleted) {
            // Tombstone (deleted) slot found - continue with probing sequence.
            continue;
        }

        if (!ht->entries[idx].key) {
            break;
        }

        // Key found
        if (strcmp(ht->entries[idx].key, lower_key) == 0) {
            // Clean up allocations.
            free(ht->entries[idx].key);
            free(ht->entries[idx].value);

            // Set values to NULL to indicate slot is empty.
            ht->entries[idx].key = NULL;
            ht->entries[idx].value = NULL;

            // Also mark entry as deleted for future lookup/deletions.
            ht->entries[idx].is_deleted = 1;

            ht->size--;
            return 1;
        }
    }

    return 0;
}

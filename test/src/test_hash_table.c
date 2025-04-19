#include "test_hash_table.h"

#define HASH_TABLE_SIZE 64
#define STRESS_TEST_OPS 1000000

static uint64_t bad_hash(const char *key, size_t key_len) {
    return (uint64_t)((unsigned char)key[0]) ^ (uint64_t)key_len;
}

void test_hash_table_init(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(ht.entries != NULL);
    assert(ht.hash_fn != NULL);
    assert(ht.capacity == HASH_TABLE_SIZE);
    assert(ht.size == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_init_with_hash(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, bad_hash));

    assert(ht.entries != NULL);
    assert(ht.hash_fn != NULL);
    assert(ht.capacity == HASH_TABLE_SIZE);
    assert(ht.size == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_insert_lookup(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key1", "value1"));
    assert(strcmp(hash_table_lookup(&ht, "key1"), "value1") == 0);

    assert(hash_table_insert(&ht, "key2", "value2"));
    assert(strcmp(hash_table_lookup(&ht, "key2"), "value2") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_delete(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key1", "value1"));
    assert(strcmp(hash_table_lookup(&ht, "key1"), "value1") == 0);

    assert(hash_table_delete(&ht, "key1"));
    assert(hash_table_lookup(&ht, "key1") == NULL);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_duplicate_insertion(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key1", "value1"));
    assert(strcmp(hash_table_lookup(&ht, "key1"), "value1") == 0);

    assert(hash_table_insert(&ht, "key1", "new_value"));
    assert(strcmp(hash_table_lookup(&ht, "key1"), "value1, new_value") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_resize(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    uint32_t local_capacity = ht.capacity;

    for (size_t i = 0; i < local_capacity * 2; i++) {
        char key[20];
        char value[20];

        snprintf(key, 20, "key%zu", i);
        snprintf(value, 20, "value%zu", i);

        assert(hash_table_insert(&ht, key, value));
    }

    assert(ht.capacity != local_capacity);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_special_characters(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key@!#$", "special_value"));
    assert(strcmp(hash_table_lookup(&ht, "key@!#$"), "special_value") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_lookup_case_insensitive(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key", "special_value"));
    assert(strcmp(hash_table_lookup(&ht, "KEY"), "special_value") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_empty_key(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "", "empty_value"));
    assert(strcmp(hash_table_lookup(&ht, ""), "empty_value") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_empty_value(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "empty_key", ""));
    assert(strcmp(hash_table_lookup(&ht, "empty_key"), "") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_stress_test_insert(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    for (size_t i = 0; i < STRESS_TEST_OPS; i++) {
        char key[20], value[20];
        snprintf(key, sizeof(key), "key%zu", i);
        snprintf(value, sizeof(value), "value%zu", i);
        assert(hash_table_insert(&ht, key, value));
    }

    assert(strcmp(hash_table_lookup(&ht, "key50000"), "value50000") == 0);
    assert(strcmp(hash_table_lookup(&ht, "key999999"), "value999999") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_repeated_deletion(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "key1", "value1"));
    assert(hash_table_delete(&ht, "key1"));
    assert(!hash_table_delete(&ht, "key1"));
    assert(hash_table_lookup(&ht, "key1") == NULL);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_lifecycle(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_insert(&ht, "temp", "val"));
    assert(strcmp(hash_table_lookup(&ht, "temp"), "val") == 0);
    assert(hash_table_delete(&ht, "temp"));
    assert(hash_table_lookup(&ht, "temp") == NULL);
    assert(hash_table_insert(&ht, "temp", "newval"));
    assert(strcmp(hash_table_lookup(&ht, "temp"), "newval") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_mass_deletion_and_reuse(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    size_t count = 10000;
    char key[32], value[32];

    for (size_t i = 0; i < count; i++) {
        snprintf(key, sizeof(key), "key%zu", i);
        snprintf(value, sizeof(value), "value%zu", i);
        assert(hash_table_insert(&ht, key, value));
    }

    for (size_t i = 0; i < count; i++) {
        snprintf(key, sizeof(key), "key%zu", i);
        assert(hash_table_delete(&ht, key));
    }

    for (size_t i = 0; i < count; i++) {
        snprintf(key, sizeof(key), "key%zu", i);
        snprintf(value, sizeof(value), "value%zu", i + 10000);
        assert(hash_table_insert(&ht, key, value));
    }

    snprintf(key, sizeof(key), "key9999");
    assert(strcmp(hash_table_lookup(&ht, key), "value19999") == 0);

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_nonexistent_keys(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    assert(hash_table_lookup(&ht, "ghost") == NULL);
    assert(!hash_table_delete(&ht, "ghost"));

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_collision_after_mass_deletion(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    size_t insert_count = ht.capacity / 2;
    for (size_t i = 0; i < insert_count; i++) {
        char key[32], value[32];
        snprintf(key, sizeof(key), "key%zu", i);
        snprintf(value, sizeof(value), "value%zu", i);
        assert(hash_table_insert(&ht, key, value));
    }

    for (size_t i = 0; i < insert_count / 2; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%zu", i * 2);
        hash_table_delete(&ht, key);
    }

    for (size_t i = 0; i < insert_count / 4; i++) {
        char key[32], value[32];
        snprintf(key, sizeof(key), "new_key%zu", i);
        snprintf(value, sizeof(value), "new_value%zu", i);
        assert(hash_table_insert(&ht, key, value));
    }

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_probing_sequence(void) {
    hash_table_t ht;
    assert(hash_table_init(&ht, HASH_TABLE_SIZE, NULL));

    for (size_t i = 0; i < ht.capacity / 16; i++) {
        char key[32], value[32];
        snprintf(key, sizeof(key), "key%zu", i);
        snprintf(value, sizeof(value), "value%zu", i);

        if (i % 4 == 0)
            hash_table_insert(&ht, "", value);
        else
            hash_table_insert(&ht, key, value);
    }

    for (size_t i = 0; i < ht.capacity / 32; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%zu", i * 4);
        hash_table_delete(&ht, key);
    }

    for (size_t i = 0; i < ht.capacity / 16; i++) {
        char key[32];
        snprintf(key, sizeof(key), "key%zu", i + ht.capacity / 8);

        if (!hash_table_lookup(&ht, key)) continue;
        assert(strcmp(hash_table_lookup(&ht, key), "value") == 0);
    }

    hash_table_free(&ht);
    printf("[PASS] %s\n", __func__);
}

void test_hash_table_all() {
    test_hash_table_init();
    test_hash_table_init_with_hash();
    test_hash_table_insert_lookup();
    test_hash_table_delete();
    test_hash_table_duplicate_insertion();
    test_hash_table_resize();
    test_hash_table_special_characters();
    test_hash_table_lookup_case_insensitive();
    test_hash_table_empty_key();
    test_hash_table_empty_value();
    test_hash_table_stress_test_insert();
    test_hash_table_repeated_deletion();
    test_hash_table_lifecycle();
    test_hash_table_mass_deletion_and_reuse();
    test_hash_table_nonexistent_keys();
    test_hash_table_collision_after_mass_deletion();
    test_hash_table_probing_sequence();
}

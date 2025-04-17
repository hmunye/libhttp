#include "test_channel.h"

// ENCODE_INT(n) allows the int to be passed "by value" through the
// channel.
//
// DECODE_INT(p) casts the void * back to intptr_t, then to the original int
// value.
//
// Note: This works only if these are the only values being written to the
// channel, the int values fit within an intptr_t, and the returned
// values are never dereferenced.
#define ENCODE_INT(n) ((void *)(intptr_t)(n))
#define DECODE_INT(p) ((int)(intptr_t)(p))

#define NUM_OPERATIONS 10000
#define CONSUMER_SIGNAL -1
#define MAX_THREADS (uint32_t)sysconf(_SC_NPROCESSORS_ONLN)

static int produced_count = 0;
static int consumed_count = 0;
static int produced_sum = 0;
static int consumed_sum = 0;
static pthread_mutex_t checksum_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *producer(void *arg) {
    channel_t *chan = (channel_t *)arg;

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        channel_write(chan, ENCODE_INT(i));
    }

    return NULL;
}

static void *consumer(void *arg) {
    channel_t *chan = (channel_t *)arg;

    while (1) {
        int data = DECODE_INT(channel_read(chan));

        if (data == CONSUMER_SIGNAL) {
            break;
        }
    }

    return NULL;
}

static void *producer_checksum(void *arg) {
    channel_t *chan = (channel_t *)arg;

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        channel_write(chan, ENCODE_INT(i));

        pthread_mutex_lock(&checksum_mutex);
        produced_count++;
        produced_sum += i;
        pthread_mutex_unlock(&checksum_mutex);
    }

    return NULL;
}

static void *consumer_checksum(void *arg) {
    channel_t *chan = (channel_t *)arg;

    while (1) {
        int data = DECODE_INT(channel_read(chan));

        if (data == CONSUMER_SIGNAL) {
            break;
        }

        pthread_mutex_lock(&checksum_mutex);
        consumed_count++;
        consumed_sum += data;
        pthread_mutex_unlock(&checksum_mutex);
    }

    return NULL;
}

static void manage_threads(pthread_t *prod_threads, pthread_t *cons_threads,
                           uint32_t prod_size, uint32_t cons_size,
                           channel_t *chan, void *(*producer_func)(void *),
                           void *(*consumer_func)(void *)) {
    for (size_t i = 0; i < prod_size; ++i) {
        assert(pthread_create(&prod_threads[i], NULL, producer_func, chan) ==
               0);
    }

    for (size_t i = 0; i < cons_size; ++i) {
        assert(pthread_create(&cons_threads[i], NULL, consumer_func, chan) ==
               0);
    }

    for (size_t i = 0; i < prod_size; ++i) {
        assert(pthread_join(prod_threads[i], NULL) == 0);
    }

    // Signal consumer threads that no more producers are writing
    for (size_t i = 0; i < cons_size; ++i) {
        channel_write(chan, ENCODE_INT(CONSUMER_SIGNAL));
    }

    for (size_t i = 0; i < cons_size; ++i) {
        assert(pthread_join(cons_threads[i], NULL) == 0);
    }
}

void test_channel_init(void) {
    channel_t *chan = channel_init(64);
    assert(chan != NULL);

    channel_free(chan, NULL);
    printf("[PASS] %s\n", __func__);
}

void test_channel_single_producer_single_consumer(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 1;
    uint32_t num_consumers = 1;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_multiple_producers_single_consumer(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 3;
    uint32_t num_consumers = 1;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_single_producer_multiple_consumers(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 1;
    uint32_t num_consumers = 3;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_multiple_producers_multiple_consumers(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 2;
    uint32_t num_consumers = 2;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_high_producer_load(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = MAX_THREADS - 3;
    uint32_t num_consumers = 1;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_high_consumer_load(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 1;
    uint32_t num_consumers = MAX_THREADS - 3;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_balanced_load(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = (MAX_THREADS / 2) - 1;
    uint32_t num_consumers = (MAX_THREADS / 2) - 1;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer, consumer);

    channel_free(chan, NULL);

    printf("[PASS] %s\n", __func__);
}

void test_channel_with_checksum(void) {
    // Using capacity of two for maximum contention
    uint32_t capacity = 2;
    uint32_t num_producers = 2;
    uint32_t num_consumers = 2;

    channel_t *chan = channel_init(capacity);
    assert(chan != NULL);

    pthread_t prod_threads[num_producers];
    pthread_t cons_threads[num_consumers];

    manage_threads(prod_threads, cons_threads, num_producers, num_consumers,
                   chan, producer_checksum, consumer_checksum);

    channel_free(chan, NULL);

    pthread_mutex_lock(&checksum_mutex);
    assert(produced_count == consumed_count);
    assert(produced_sum == consumed_sum);
    pthread_mutex_unlock(&checksum_mutex);

    printf("[PASS] %s\n", __func__);
}

void test_channel_all(void) {
    test_channel_init();
    test_channel_single_producer_single_consumer();
    test_channel_multiple_producers_single_consumer();
    test_channel_single_producer_multiple_consumers();
    test_channel_multiple_producers_multiple_consumers();
    test_channel_high_producer_load();
    test_channel_high_consumer_load();
    test_channel_balanced_load();
    test_channel_with_checksum();
}

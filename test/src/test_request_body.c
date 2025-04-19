#include "test_request_body.h"

#define MAX_BYTES_PER_READ 10

void test_request_body_valid(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Content-Length: 13\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello, World!\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_OK);
    assert(strcmp(req.body, "Hello, World!") == 0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_valid_truncated(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "POST /submit HTTP/1.1\r\n"
            "Content-Length: 10\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello, World! This is too long.\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_OK);
    assert(strcmp(req.body, "Hello, Wor") == 0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_valid_no_content_length(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_OK);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_valid_content_length_zero(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Content-Length: 0\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_OK);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_invalid_short_body(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "POST /submit HTTP/1.1\r\n"
            "Content-Length: 15\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello, Wo\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

// Shouldn't be invalid, we're assuming Content-Length will be present if a body
// exists.
void test_request_body_valid_no_content_length_with_body(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "POST /submit HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello, Wo\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_OK);
    assert(strcmp(req.body, "") == 0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_invalid_content_length(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "POST /submit HTTP/1.1\r\n"
            "Content-Length: -9sdas\r\n"
            "Host: example.com\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Some data\r\n",
        .bytes_per_read = 1,
        .pos = 0,
    };

    char buf[strlen(reader.data) + 1];

    int status;
    size_t bytes_read;
#ifdef _DEBUG
    size_t iteration = 0;
#endif
    while (1) {
        // Compute a random number between 1 and MAX_BYTES_PER_READ, inclusive,
        // for each read.
        reader.bytes_per_read = (size_t)((rand() % MAX_BYTES_PER_READ) + 1);

        bytes_read = chunk_reader_read(&reader, buf, sizeof buf);
        if (bytes_read == 0) {
            while ((status = request_parse(&req, "", 0)) == PARSE_INCOMPLETE);
            break;
        }

#ifdef _DEBUG
        printf("[%02zu] bytes-read (%02zu): ", iteration++, bytes_read);
        print_escaped_buf(buf, bytes_read);
        printf("\n");
#endif

        if ((status = request_parse(&req, buf, bytes_read)) !=
            PARSE_INCOMPLETE) {
            break;
        }
    }

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_body_all(void) {
    test_request_body_valid();
    test_request_body_valid_truncated();
    test_request_body_valid_no_content_length();
    test_request_body_valid_content_length_zero();
    test_request_body_invalid_short_body();
    test_request_body_valid_no_content_length_with_body();
    test_request_body_invalid_content_length();
}

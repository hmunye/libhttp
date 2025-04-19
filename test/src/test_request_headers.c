#include "test_request_headers.h"

#define MAX_BYTES_PER_READ 10

void test_request_headers_valid_single(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: localhost:4040\r\n"
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
    assert(strcmp(hash_table_lookup(req.headers, "host"), "localhost:4040") ==
           0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_valid_multiple(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "User-Agent: Mozilla/5.0\r\n"
            "Accept: text/html\r\n"
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
    assert(strcmp(hash_table_lookup(req.headers, "host"), "example.com") == 0);
    assert(strcmp(hash_table_lookup(req.headers, "user-agent"),
                  "Mozilla/5.0") == 0);
    assert(strcmp(hash_table_lookup(req.headers, "accept"), "text/html") == 0);
    assert(strcmp(hash_table_lookup(req.headers, "connection"), "keep-alive") ==
           0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_valid_spacing(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host:    example.com    \r\n"
            "User-Agent:    Mozilla/5.0\r\n"
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
    assert(strcmp(hash_table_lookup(req.headers, "host"), "example.com") == 0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_spacing(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "  Host : example.com\r\n"
            "User-Agent:    Mozilla/5.0\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_missing_colon(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host example.com\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_empty_value(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: \r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_empty_name(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            ": example.com\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_lf(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "X-Newline-Header: value\nmore_data\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_cr(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "X-Ne\rline-Header: value_more_data\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_valid_duplicate_header(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Cookie: 1a\r\n"
            "Cookie: 1b\r\n"
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
    assert(strcmp(hash_table_lookup(req.headers, "host"), "example.com") == 0);
    assert(strcmp(hash_table_lookup(req.headers, "cookie"), "1a, 1b") == 0);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_missing_crlf(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_long_name(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "X-This-Is-A-Very-Long-Header-Name-"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA: "
            "value\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_long_value(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "X-This-Is-A-Very-Long-Header-Value: "
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_multiple_crlf(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "X-Malfor\r\nmed-Header: value\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_value_characters(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "X-Control-Header: \x07\x0A\x1B\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_name_characters(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "X-Con\x07\x0A-Header: hello\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_invalid_headers_limit(void) {
    request_t req = {
        .request_line = {0},
        .headers = hash_table_init(64, NULL),
    };
    assert(req.headers);

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
            "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 "
            "Safari/537.36\r\n"
            "Accept: "
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
            "webp,image/apng,*/*;q=0.8\r\n"
            "Accept-Encoding: gzip, deflate, br\r\n"
            "Accept-Language: en-US,en;q=0.9\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: 1234\r\n"
            "Content-Type: application/json\r\n"
            "DNT: 1\r\n"
            "Expect: 100-continue\r\n"
            "From: user@example.com\r\n"
            "If-Modified-Since: Mon, 27 Sep 2021 13:00:00 GMT\r\n"
            "If-None-Match: \"etag_value\"\r\n"
            "Origin: https://www.example.com\r\n"
            "Pragma: no-cache\r\n"
            "Range: bytes=0-1023\r\n"
            "Referer: https://www.example.com/page\r\n"
            "TE: Trailers\r\n"
            "Upgrade-Insecure-Requests: 1\r\n"
            "X-Requested-With: XMLHttpRequest\r\n"
            "X-Frame-Options: DENY\r\n"
            "X-XSS-Protection: 1; mode=block\r\n"
            "X-Content-Type-Options: nosniff\r\n"
            "X-RateLimit-Limit: 1000\r\n"
            "X-RateLimit-Remaining: 999\r\n"
            "X-RateLimit-Reset: 1615560000\r\n"
            "X-Api-Key: abc123xyz456\r\n"
            "X-Correlation-ID: 1234567890abcdef\r\n"
            "X-Forwarded-For: 192.168.1.1\r\n"
            "X-Forwarded-Proto: https\r\n"
            "X-Real-IP: 192.168.1.1\r\n"
            "X-Content-Duration: 300\r\n"
            "X-Content-Encoding: gzip\r\n"
            "X-Request-ID: 9876543210abcdef\r\n"
            "X-Frame-Option: SAMEORIGIN\r\n"
            "X-Authorization: Bearer token\r\n"
            "X-Response-Time: 50ms\r\n"
            "X-Service-Name: MyService\r\n"
            "X-Env: production\r\n"
            "X-Error-Code: 400\r\n"
            "X-Message: Request was malformed\r\n"
            "X-Status: OK\r\n"
            "X-Sub-Status: None\r\n"
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

    assert(status == PARSE_INVALID);

    hash_table_free(req.headers);
    printf("[PASS] %s\n", __func__);
}

void test_request_headers_all(void) {
    test_request_headers_valid_single();
    test_request_headers_valid_multiple();
    test_request_headers_valid_spacing();
    test_request_headers_invalid_spacing();
    test_request_headers_invalid_missing_colon();
    test_request_headers_invalid_empty_value();
    test_request_headers_invalid_empty_name();
    test_request_headers_invalid_lf();
    test_request_headers_invalid_cr();
    test_request_headers_valid_duplicate_header();
    test_request_headers_invalid_missing_crlf();
    test_request_headers_invalid_long_name();
    test_request_headers_invalid_long_value();
    test_request_headers_invalid_multiple_crlf();
    test_request_headers_invalid_value_characters();
    test_request_headers_invalid_name_characters();
    test_request_headers_invalid_headers_limit();
}

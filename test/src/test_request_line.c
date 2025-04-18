#include "test_request_line.h"

#define MAX_BYTES_PER_READ 3

void test_request_line_valid_get(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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
    assert(req.request_line.method == GET);
    assert(strcmp(req.request_line.request_target, "/") == 0);
    assert(strcmp(req.request_line.version, "HTTP/1.1") == 0);

    printf("[PASS] %s\n", __func__);
}

void test_request_line_valid_get_with_path(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET /index.html HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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
    assert(req.request_line.method == GET);
    assert(strcmp(req.request_line.request_target, "/index.html") == 0);
    assert(strcmp(req.request_line.version, "HTTP/1.1") == 0);

    printf("[PASS] %s\n", __func__);
}

void test_request_line_valid_post_with_path(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "POST /submit-form HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "\r\n"
            "field1=value1",
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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
    assert(req.request_line.method == POST);
    assert(strcmp(req.request_line.request_target, "/submit-form") == 0);
    assert(strcmp(req.request_line.version, "HTTP/1.1") == 0);

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_missing_parts(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / \r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_out_of_order(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "/ GET HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_version(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.2\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_method(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "FOO / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_long_method(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GETTTTTTTTTTTTTTTTTTTTTTT / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_long_request_target(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET "
            "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaa HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_extra_whitespace(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET      /      HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_tab_replace_sp(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET\t/index HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_malformed_version(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_missing_crlf(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
            while ((status = request_parse(&req, "", 0)) != PARSE_INCOMPLETE);
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_null_bytes(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "G\x00T /\x00 HTTP\x00/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_incomplete_request_line(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data = "GET / HT",
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_leading_crlf(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "\r\nGET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_control_character(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GE\x01T / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_utf_8_request_line(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "\xC2\xA9 / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_version_injection(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\rHTTP/1.0\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

// TODO: Ensure this test is invalid after implementing rest of parser
void test_request_line_invalid_multiple_request_lines(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\nPOST /hack HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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
    assert(req.request_line.method == GET);
    assert(strcmp(req.request_line.request_target, "/") == 0);
    assert(strcmp(req.request_line.version, "HTTP/1.1") == 0);

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_data_after_version(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1 some junk\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_cr_abuse(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\r\r\r\n"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_invalid_lfcr(void) {
    request_t req = {0};

    chunk_reader_t reader = {
        .data =
            "GET / HTTP/1.1\n\r"
            "Host: localhost\r\n"
            "User-Agent: curl\r\n"
            "Accept: */*\r\n"
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

    printf("[PASS] %s\n", __func__);
}

void test_request_line_all(void) {
    srand((unsigned)time(NULL));

    test_request_line_valid_get();
    test_request_line_valid_get_with_path();
    test_request_line_valid_post_with_path();
    test_request_line_invalid_missing_parts();
    test_request_line_invalid_out_of_order();
    test_request_line_invalid_version();
    test_request_line_invalid_method();
    test_request_line_invalid_long_method();
    test_request_line_invalid_long_request_target();
    test_request_line_invalid_extra_whitespace();
    test_request_line_invalid_tab_replace_sp();
    test_request_line_invalid_malformed_version();
    test_request_line_invalid_missing_crlf();
    test_request_line_invalid_null_bytes();
    test_request_line_invalid_incomplete_request_line();
    test_request_line_invalid_leading_crlf();
    test_request_line_invalid_control_character();
    test_request_line_invalid_utf_8_request_line();
    test_request_line_invalid_version_injection();
    test_request_line_invalid_multiple_request_lines();
    test_request_line_invalid_data_after_version();
    test_request_line_invalid_cr_abuse();
    test_request_line_invalid_lfcr();
}

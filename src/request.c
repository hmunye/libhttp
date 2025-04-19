#include "request.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// State machine approach for incremental parsing of the request.
typedef enum {
    PARSER_RL = 0, /* State for parsing the request line. */
    PARSER_H,      /* State for parsing the headers. */
} parser_state_t;

typedef struct {
    char buf[REQUEST_LINE_SIZE]; /* Buffer for accumulating parts of the
                                    request. */
    size_t bytes_read;           /* Number of bytes read so far. */
    parser_state_t parser_state; /* Current state of the parser. */
} parser_t;

// Must be thread-local if shared globally, using GCC-specific extension.
__thread parser_t parser = {0};

// Reset the parser to its default state after parsing since the state lasts
// for the lifetime of the thread using it.
static void parser_reset(void) {
    memset(&parser, 0, sizeof parser);
}

const char *method_to_str[] = {
    "GET",     /* GET */
    "HEAD",    /* HEAD */
    "POST",    /* POST */
    "PUT",     /* PUT */
    "DELETE",  /* DELETE */
    "OPTIONS", /* OPTIONS */
    "UNKNOWN"  /* UNKNOWN_METHOD */
};

static method_t str_to_method(const char *method_str) {
    if (strcmp(method_str, "GET") == 0) return GET;
    if (strcmp(method_str, "HEAD") == 0) return HEAD;
    if (strcmp(method_str, "POST") == 0) return POST;
    if (strcmp(method_str, "PUT") == 0) return PUT;
    if (strcmp(method_str, "DELETE") == 0) return DELETE;
    if (strcmp(method_str, "OPTIONS") == 0) return OPTIONS;

    return UNKNOWN_METHOD;
}

// Parse the given line, populating the headers of `req`.
static int request_header_parse(request_t *req, char *line, size_t line_len) {
    (void)req;
    char *field_name_end = line;
    while (line_len-- > 0 && *field_name_end != ':' && *field_name_end != ' ') {
        field_name_end++;
    }

    // Checks if first character is SP. Also no SP allowed between field-name
    // and colon. `field_name_end` should be pointing to the colon on valid
    // header.
    if (line_len == 0 || *field_name_end != ':') {
        return PARSE_INVALID;
    }

    // ALL characters between first to before the colon, inclusive.
    size_t field_name_len = (size_t)(field_name_end - line);
    if (field_name_len > HEADER_FIELD_NAME_SIZE) {
        return PARSE_INVALID;
    }

    // Skip any leading whitespace of the field-value.
    while (line_len-- > 0 && *(++field_name_end) == ' ');

    if (line_len == 0) {
        return PARSE_INVALID;
    }

    // +1 to also capture the LF.
    char *field_value_end = field_name_end + line_len + 1;

    // Trim any trailing whitespace of the field-value.
    while ((field_value_end--) > field_name_end &&
           (*field_value_end == ' ' || *field_value_end == '\r' ||
            *field_value_end == '\n'));

    // Replace `\r` with `\0`.
    *(field_value_end + 1) = '\0';

    // ALL characters between first character after leading whitespace character
    // before trailing whitespace and CRLF, inclusive.
    size_t field_value_len = (size_t)(field_value_end - field_name_end);
    if (field_value_len > HEADER_FIELD_VALUE_SIZE) {
        return PARSE_INVALID;
    }

    // TODO: insert values into headers hash table
    line[field_name_len] = '\0';
    printf("header name: %s\n", line);

    printf("header value: %s\n", field_name_end);

    return PARSE_OK;
}

// Parse the given line, populating the request-line of `req`.
static int request_line_parse(request_t *req, char *line, size_t line_len) {
    char *method_end = line;
    while (line_len-- > 0 && *method_end != ' ') {
        method_end++;
    }

    if (line_len == 0) {
        return PARSE_INVALID;
    }

    size_t method_len = (size_t)(method_end - line);
    if (method_len > METHOD_SIZE) {
        return PARSE_INVALID;
    }

    method_t method;
    line[method_len] = '\0';
    if ((method = str_to_method(line)) == UNKNOWN_METHOD) {
        return PARSE_INVALID;
    }

    req->request_line.method = method;

    char *request_target_start = method_end + 1;
    char *request_target_end = request_target_start;
    while (line_len-- > 0 && *request_target_end != ' ') {
        request_target_end++;
    }

    if (line_len == 0) {
        return PARSE_INVALID;
    }

    size_t request_target_len =
        (size_t)(request_target_end - request_target_start);
    if (request_target_len >= sizeof req->request_line.request_target) {
        return PARSE_INVALID;
    }

    memcpy(req->request_line.request_target, request_target_start,
           request_target_len);
    req->request_line.request_target[request_target_len] = '\0';

    if (strcmp(req->request_line.request_target, "") == 0) {
        return PARSE_INVALID;
    }

    char *version_start = request_target_end + 1;
    char *version_end = version_start;
    while (line_len-- > 0 && *version_end != '\r' &&
           *(version_end + 1) != '\n') {
        version_end++;
    }

    // After locating the CR in a CRLF, the remaining line should only contain
    // the LF. If there's more than one byte left, the request line has extra
    // data and is invalid.
    if (line_len != 1) {
        return PARSE_INVALID;
    }

    size_t version_len = (size_t)(version_end - version_start);
    if (version_len >= sizeof req->request_line.version) {
        return PARSE_INVALID;
    }

    memcpy(req->request_line.version, version_start, version_len);
    req->request_line.version[version_len] = '\0';

    // Only supporting HTTP/1.1
    if (strcmp(req->request_line.version, "HTTP/1.1") != 0) {
        return PARSE_INVALID;
    }

    return PARSE_OK;
}

int request_parse(request_t *req, char *chunk, size_t chunk_len) {
    assert(req && chunk);

    if (chunk_len == 0) {
        // When the chunk is empty, parse and process the remaining contents of
        // `parser.buf`. Empty `chunk` should represent end of stream.
        goto empty_chunk;
    }

    size_t total_bytes = chunk_len + parser.bytes_read;

    if (total_bytes > REQUEST_LINE_SIZE) {
        parser_reset();
        return PARSE_INVALID;
    }

    memcpy(parser.buf + parser.bytes_read, chunk, chunk_len);
    parser.buf[total_bytes] = '\0';
    parser.bytes_read = total_bytes;

    char *end;
empty_chunk:
    if ((end = strstr(parser.buf, "\r\n")) == NULL) {
        if (chunk_len == 0) {
            parser_reset();
            return PARSE_INVALID;
        }

        return PARSE_INCOMPLETE; /* Need more data to process a full line. */
    }

    // Treat this part of the buffer, including CRLF, as a complete line.
    size_t line_len = (size_t)(end - parser.buf + 2);
    char *line = parser.buf;

    switch (parser.parser_state) {
        case PARSER_RL: {
            int status;
            if ((status = request_line_parse(req, line, line_len)) !=
                PARSE_OK) {
                parser_reset();
                return status;
            }

            // Shifting the unprocessed bytes in `parser.buf` to the front, so
            // the buffer can continue to be filled and parsed incrementally
            // without losing data.
            parser.bytes_read -= line_len;
            memmove(parser.buf, parser.buf + line_len, parser.bytes_read);
            parser.buf[parser.bytes_read] = '\0';

            // Transition to next state (headers).
            parser.parser_state = PARSER_H;
            return PARSE_INCOMPLETE;
        }
        case PARSER_H: {
            // Check for empty line (single CRLF) that indicates end of HTTP
            // headers section.
            if (parser.bytes_read > 1 &&
                (parser.buf[0] == '\r' && parser.buf[1] == '\n')) {
                // Move leading CRLF out of buffer
                memmove(parser.buf, parser.buf + 2, parser.bytes_read - 1);
                parser.bytes_read -= 2;

                // TODO: Transition to next state
                break;
            }

            // TODO: check if max headers limit is reached before parsing

            int status;
            if ((status = request_header_parse(req, line, line_len)) !=
                PARSE_OK) {
                parser_reset();
                return status;
            }

            parser.bytes_read -= line_len;
            memmove(parser.buf, parser.buf + line_len, parser.bytes_read);
            parser.buf[parser.bytes_read] = '\0';

            // Need more data to process all field-lines (headers).
            return PARSE_INCOMPLETE;
        }
        default: {
            fprintf(stderr, "ERROR: request_parse: invalid parser state.\n");
            parser_reset();
            return PARSE_ERR;
        }
    }

    parser_reset();
    return PARSE_OK;
}

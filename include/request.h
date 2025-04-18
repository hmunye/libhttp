#ifndef REQUEST_H
#define REQUEST_H

#include <stdlib.h>

#ifndef PARSE_OK
#define PARSE_OK 0 /* Parsing completed successfully without issues. */
#endif

#ifndef PARSE_ERR
#define PARSE_ERR -1 /* An error occurred during the parsing process. */
#endif

#ifndef PARSE_INCOMPLETE
#define PARSE_INCOMPLETE -2 /* More data is required to complete parsing. */
#endif

#ifndef PARSE_INVALID
#define PARSE_INVALID -3 /* The parsed data is invalid or malformed. */
#endif

/* RFC 9112:
 * 3. Request Line
 *
 * It is RECOMMENDED that all HTTP senders and recipients support, at a minimum,
 * request-line lengths of 8000 octets.
 * */
#define METHOD_SIZE 7 /* Enough for the largest HTTP method supported here. */
#define REQUEST_TARGET_SIZE 1025
#define VERSION_SIZE 9 /* Only supporting HTTP/1.1 version. */

/* Extra space for space characters (SP) and (CRLF). */
#define REQUEST_LINE_SIZE (METHOD_SIZE + REQUEST_TARGET_SIZE + VERSION_SIZE + 4)

/* RFC 9110:
 * 9. Methods
 *
 * All general-purpose servers MUST support the methods GET and HEAD. All other
 * methods are OPTIONAL. */
typedef enum {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    OPTIONS,
    UNKNOWN_METHOD,
} method_t;

/* Lookup table to convert `method_t` -> `char *`. */
extern const char *method_to_str[];

typedef struct {
    method_t method;
    char request_target[REQUEST_TARGET_SIZE];
    char version[VERSION_SIZE];
} request_line_t;

typedef struct {
    request_line_t request_line;
} request_t;

/* Parses HTTP request chunks incrementally into the given `req`. Returns
 * one of (PARSE_OK), (PARSE_ERR), (PARSE_INCOMPLETE), or (PARSE_INVALID),
 * indicating status of parsing. */
int request_parse(request_t *req, char *chunk, size_t chunk_len);

#endif  // REQUEST_H

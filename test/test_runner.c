#include "test_channel.h"
#include "test_hash_table.h"
#include "test_request_headers.h"
#include "test_request_line.h"

int main(void) {
    printf("+-------------------+\n");
    printf("|   CHANNEL TESTS   |\n");
    printf("+-------------------+\n");
    test_channel_all();

    printf("+----------------------+\n");
    printf("|   HASH TABLE TESTS   |\n");
    printf("+----------------------+\n");
    test_hash_table_all();

    printf("+--------------------------------+\n");
    printf("|   REQUEST LINE PARSING TESTS   |\n");
    printf("+--------------------------------+\n");
    test_request_line_all();

    printf("+-----------------------------------+\n");
    printf("|   REQUEST HEADERS PARSING TESTS   |\n");
    printf("+-----------------------------------+\n");
    test_request_headers_all();

    return 0;
}

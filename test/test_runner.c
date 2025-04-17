#include "test_channel.h"
#include "test_request_line.h"

int main(void) {
    printf("+-------------------+\n");
    printf("|   CHANNEL TESTS   |\n");
    printf("+-------------------+\n");
    test_channel_all();

    printf("+------------------------+\n");
    printf("|   REQUEST LINE TESTS   |\n");
    printf("+------------------------+\n");
    test_request_line_all();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 8

int main(void) {
    int ret;
    FILE *file = fopen("messages.txt", "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buf[BUF_SIZE + 1];
    size_t bytes_read;
    while ((bytes_read = fread(buf, sizeof(*buf), BUF_SIZE, file)) > 0) {
        buf[bytes_read] = '\0';
        printf("read: %s\n", buf);
    }

    if (feof(file)) {
        ret = 0;
        goto cleanup;
    }

    if (ferror(file)) {
        fprintf(stderr, "ERROR: failed to read from file");
        ret = 1;
        goto cleanup;
    }

cleanup:
    fclose(file);
    return ret;
}

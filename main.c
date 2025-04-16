#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 8
#define LINE_SIZE 1024

int main(void) {
    int ret;
    FILE *file = fopen("messages.txt", "r");
    if (!file) {
        perror("ERROR: fopen");
        exit(1);
    }

    char buf[BUF_SIZE + 1];
    size_t bytes_read;
    char line[LINE_SIZE + 1];
    size_t line_idx;
    while ((bytes_read = fread(buf, sizeof(*buf), BUF_SIZE, file)) > 0) {
        buf[bytes_read] = '\0';
        for (size_t i = 0; i < bytes_read; ++i) {
            if (buf[i] == '\n') {
                line[line_idx] = '\0';

                printf("read: %s\n", line);

                line[0] = '\0';
                line_idx = 0;
            } else {
                if (line_idx < LINE_SIZE) {
                    line[line_idx++] = buf[i];
                }
            }
        }
    }

    if (line_idx > 0) {
        line[line_idx] = '\0';
        printf("read: %s\n", line);
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

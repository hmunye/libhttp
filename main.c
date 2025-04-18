#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "channel.h"
#include "listener.h"
#include "request.h"

// ENCODE_INT(n): int -> intptr_t -> void *
#define ENCODE_INT(n) ((void *)(intptr_t)(n))
// DECODE_INT(p): void * -> intptr_t -> int
#define DECODE_INT(p) ((int)(intptr_t)(p))

#define HOST "127.0.0.1"
#define PORT "8080"
#define BACKLOG 16 /* Maximum number of pending connections in the queue. */

// Timeout in milliseconds.
#define POLLING_TIMEOUT 5

#define BUFFER_SIZE 8

#define THREAD_POOL 8
#define CHANNEL_SIZE 16
channel_t *chan;

// TODO: replace polling since it is only on one socket at a time
// consumer: reads client connections from channel and writes lines to stdout.
static void *producer(void *arg) {
    (void)arg;

    while (1) {
        int clientfd = DECODE_INT(channel_read(chan));

        request_t req = {0};
        char buf[BUFFER_SIZE + 1];

        struct pollfd pfd[1];

        pfd[0].fd = clientfd;
        pfd[0].events = POLLIN;

        int status, events;
        ssize_t bytes_read;
        while (1) {
            // Poll client socket for reading to avoid blocking on recv().
            events = poll(pfd, 1, POLLING_TIMEOUT);
            if (events == -1) {
                break;
            }

            // Timeout expired.
            if (events == 0) {
                while ((status = request_parse(&req, "", 0)) ==
                       PARSE_INCOMPLETE);
                break;
            }

            // Check if POLLIN is set and data is ready to read.
            if (pfd[0].revents & POLLIN) {
                bytes_read = recv(clientfd, buf, (sizeof buf) - 1, 0);
                if (bytes_read <= 0) {
                    break;
                }

                buf[bytes_read] = '\0';
                if ((status = request_parse(&req, buf, (size_t)bytes_read)) !=
                    PARSE_INCOMPLETE) {
                    break;
                }
            }
        }

        if (events == -1) {
            perror("ERROR: poll");

            printf("server: client connection closed\n");
            close(clientfd);
            continue;
        }

        if (bytes_read == -1) {
            perror("ERROR: recv");

            printf("server: client connection closed\n");
            close(clientfd);
            continue;
        }

        switch (status) {
            case PARSE_OK:
                printf("Request Line: \n");
                printf("- Method: %s\n",
                       method_to_str[req.request_line.method]);
                printf("- Target: %s\n", req.request_line.request_target);
                printf("- Version: %s\n", req.request_line.version);

                printf("server: client connection closed\n");
                close(clientfd);
                continue;
            case PARSE_ERR:
                printf("server: server error occured\n");

                printf("server: client connection closed\n");
                close(clientfd);
                continue;
            case PARSE_INCOMPLETE:
            case PARSE_INVALID:
                printf("server: error occured parsing HTTP request\n");

                printf("server: client connection closed\n");
                close(clientfd);
                continue;
        }
    }

    return NULL;
}

// producer: accepts incoming connections and writes connections to channel.
int main(void) {
    // Disable buffering for stdout (line-buffered by default).
    setbuf(stdout, NULL);

    listener_t *listener = &tcp_listener;
    connection_t conn;

    chan = channel_init(CHANNEL_SIZE);
    if (!chan) {
        exit(1);
    }

    listener->listen(HOST, PORT, BACKLOG);

    printf("server: [%s:%s] waiting for connections...\n", HOST, PORT);

    pthread_t cons_threads[THREAD_POOL];

    for (size_t i = 0; i < THREAD_POOL; ++i) {
        if (pthread_create(&cons_threads[i], NULL, producer, NULL) != 0) {
            perror("ERROR: pthread_create");
            channel_free(chan, NULL);
            listener->close();
            exit(1);
        }
    }

    while (1) {
        if (listener->accept(&conn) == -1) {
            continue;
        }

        printf("server: got connection from %s\n", conn.remote_addr);

        channel_write(chan, ENCODE_INT(conn.clientfd));
    }

    for (size_t i = 0; i < THREAD_POOL; ++i) {
        if (pthread_join(cons_threads[i], NULL) != 0) {
            fprintf(stderr, "ERROR: pthread_join: failed to join thread.\n");
            channel_free(chan, NULL);
            listener->close();
            exit(1);
        }
    }

    channel_free(chan, NULL);
    listener->close();
    return 0;
}

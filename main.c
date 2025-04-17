#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "channel.h"
#include "listener.h"

// ENCODE_INT(n): int -> intptr_t -> void *
#define ENCODE_INT(n) ((void *)(intptr_t)(n))
// DECODE_INT(p): void * -> intptr_t -> int
#define DECODE_INT(p) ((int)(intptr_t)(p))

#define HOST "127.0.0.1"
#define PORT "8080"
// Maximum number of pending connections in the queue
#define BACKLOG 16

// Timeout in milliseconds
#define POLLING_TIMEOUT 5

#define BUFFER_SIZE 8
#define LINE_SIZE 1024

#define THREAD_POOL 8
#define CHANNEL_SIZE 16
channel_t *chan;

// consumer: reads client connections from channel and writes lines to stdout
static void *producer(void *arg) {
    (void)arg;

    while (1) {
        int clientfd = DECODE_INT(channel_read(chan));

        char buf[BUFFER_SIZE + 1];
        char line[LINE_SIZE + 1];

        struct pollfd pfd[1];

        pfd[0].fd = clientfd;
        pfd[0].events = POLLIN;

        int events;
        ssize_t bytes_read;
        size_t line_idx = 0;
        while (1) {
            // Poll client socket for reading to avoid blocking on recv()
            events = poll(pfd, 1, POLLING_TIMEOUT);
            if (events == -1) {
                break;
            }

            // Timeout expired
            if (events == 0) {
                break;
            }

            // TODO: handle event errors
            if (pfd[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                break;
            }

            // Check if POLLIN is set and data is ready to read
            if (pfd[0].revents & POLLIN) {
                bytes_read = recv(clientfd, buf, (sizeof buf) - 1, 0);
                if (bytes_read <= 0) {
                    break;
                }

                buf[bytes_read] = '\0';
                for (ssize_t i = 0; i < bytes_read; ++i) {
                    if (buf[i] == '\n') {
                        line[line_idx] = '\0';
                        printf("%s\n", line);
                        line_idx = 0;
                    } else {
                        if (line_idx < LINE_SIZE) {
                            line[line_idx++] = buf[i];
                        }
                    }
                }
            }
        }

        if (events == -1) {
            perror("ERROR: poll");
            close(clientfd);
            continue;
        }

        if (bytes_read == -1) {
            perror("ERROR: recv");
            close(clientfd);
            continue;
        }

        // Timeout expired or remote side has closed the connection
        if (events == 0 || bytes_read == 0) {
            if (line_idx > 0) {
                line[line_idx] = '\0';
                printf("%s\n", line);
                line_idx = 0;
            }

            printf("server: client connection closed\n");
            close(clientfd);
        }
    }

    return NULL;
}

// producer: accepts incoming connections and writes connections to channel
int main(void) {
    // Disable buffering for stdout (line-buffered by default)
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

// Exposes POSIX definitions like getaddrinfo(), struct addrinfo, etc.
#if defined(__unix__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L
#endif

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "channel.h"

// ENCODE_INT(n): int -> intptr_t -> void *
#define ENCODE_INT(n) ((void *)(intptr_t)(n))
// DECODE_INT(p): void * -> intptr_t -> int
#define DECODE_INT(p) ((int)(intptr_t)(p))

#define HOST "127.0.0.1"
#define PORT "8080"
// Maximum number of pending connections in the queue
#define BACKLOG 10

#define BUFFER_SIZE 8
#define LINE_SIZE 1024

#define CHANNEL_SIZE 2
channel_t *chan;

// consumer: reads client connections from channel and writes lines to stdout
static void *producer(void *arg) {
    (void)arg;

    while (1) {
        int clientfd = DECODE_INT(channel_read(chan));

        char buf[BUFFER_SIZE + 1];
        char line[LINE_SIZE + 1];

        size_t line_idx = 0;
        ssize_t bytes_read;
        while ((bytes_read = recv(clientfd, buf, (sizeof buf) - 1, 0)) > 0) {
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

        if (bytes_read == -1) {
            perror("ERROR: recv");
            close(clientfd);
            continue;
        }

        // Remote side has closed the connection
        if (bytes_read == 0) {
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

    int serverfd, clientfd, status, reuse = 1;
    struct addrinfo hints, *serverinfo, *curr_node;
    struct sockaddr_storage
        client_addr;  // Large enough to hold either IPv4 (sockaddr_in) or IPv6
                      // (sockaddr_in6) address information
    socklen_t sin_size;
    char addr_buf[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      // Either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP socket

    if ((status = getaddrinfo(HOST, PORT, &hints, &serverinfo)) != 0) {
        fprintf(stderr, "ERROR: getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Iterate through the linked list of addrinfo structs and try to bind to
    // the first working socket
    for (curr_node = serverinfo; curr_node != NULL;
         curr_node = curr_node->ai_next) {
        if ((serverfd = socket(curr_node->ai_family, curr_node->ai_socktype,
                               curr_node->ai_protocol)) == -1) {
            perror("ERROR: socket");
            continue;
        }

        // Enable SO_REUSEADDR to allow the socket address to be reused after a
        // restart
        if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof reuse) == -1) {
            perror("ERROR: setsockopt");
            exit(1);
        }

        if (bind(serverfd, curr_node->ai_addr, curr_node->ai_addrlen) == -1) {
            perror("ERROR: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    // Reached end of addrinfo list without finding a valid socket
    if (!curr_node) {
        fprintf(stderr, "ERROR: server: failed to bind to socket.\n");
        exit(1);
    }

    if (listen(serverfd, BACKLOG) == -1) {
        perror("ERROR: listen");
        exit(1);
    }

    chan = channel_init(CHANNEL_SIZE);
    if (!chan) {
        close(serverfd);
        exit(1);
    }

    printf("server: [%s:%s] waiting for connections...\n", HOST, PORT);

    pthread_t cons_thread;
    if (pthread_create(&cons_thread, NULL, producer, NULL) != 0) {
        perror("ERROR: pthread_create");
        channel_free(chan, NULL);
        close(serverfd);
        exit(1);
    }

    while (1) {
        sin_size = sizeof client_addr;
        clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &sin_size);
        if (clientfd == -1) {
            perror("ERROR: accept");
            continue;
        }

        switch (client_addr.ss_family) {
            case AF_INET:  // IPv4
                inet_ntop(
                    AF_INET,
                    &(((struct sockaddr_in *)(struct sockaddr *)&client_addr)
                          ->sin_addr),
                    addr_buf, sizeof addr_buf);
                break;
            case AF_INET6:  // IPv6
                inet_ntop(
                    AF_INET6,
                    &(((struct sockaddr_in6 *)(struct sockaddr *)&client_addr)
                          ->sin6_addr),
                    addr_buf, sizeof addr_buf);
                break;
        }

        printf("server: got connection from %s\n", addr_buf);

        channel_write(chan, ENCODE_INT(clientfd));
    }

    if (pthread_join(cons_thread, NULL) != 0) {
        fprintf(stderr, "ERROR: pthread_join: failed to join thread.\n");
        channel_free(chan, NULL);
        close(serverfd);
        exit(1);
    }

    channel_free(chan, NULL);
    close(serverfd);
    return 0;
}

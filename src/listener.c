#include "listener.h"

#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int tcp_sock_fd = -1;

// Initialize a TCP socket, bind it to the specified host and port, and listen
// with the given backlog. Use NULL for host to bind to INADDR_ANY.
static void listen_tcp(const char *host, const char *port, int backlog) {
    assert(port && backlog > 0);

    int status, reuse = 1;
    struct addrinfo hints, *serverinfo, *curr_node;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     /* Either IPv4 or IPv6. */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket. */
    hints.ai_flags = AI_PASSIVE; /* Uses wildcard address if host is NULL. */

    if ((status = getaddrinfo(host, port, &hints, &serverinfo)) != 0) {
        fprintf(stderr, "ERROR: getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // Iterate through the linked list of addrinfo structs and try to bind to
    // the first working socket.
    for (curr_node = serverinfo; curr_node != NULL;
         curr_node = curr_node->ai_next) {
        if ((tcp_sock_fd = socket(curr_node->ai_family, curr_node->ai_socktype,
                                  curr_node->ai_protocol)) == -1) {
            perror("ERROR: socket");
            continue;
        }

        // Enable SO_REUSEADDR to allow the socket address to be reused after a
        // restart.
        if (setsockopt(tcp_sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof reuse) == -1) {
            perror("ERROR: setsockopt");
            exit(1);
        }

        if (bind(tcp_sock_fd, curr_node->ai_addr, curr_node->ai_addrlen) ==
            -1) {
            perror("ERROR: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    // Reached end of addrinfo list without finding a valid socket.
    if (!curr_node) {
        fprintf(stderr, "ERROR: server: failed to bind to socket.\n");
        exit(1);
    }

    if (listen(tcp_sock_fd, backlog) == -1) {
        perror("ERROR: listen");
        exit(1);
    }
}

// Wait for and accept incoming connections on the listening socket. Fills
// `conn` with the peer's connection information. Returns (0) on successful
// acceptance, otherwise (-1).
static int accept_tcp(connection_t *conn) {
    assert(conn);

    // Large enough to hold either IPv4 (sockaddr_in) or IPv6 (sockaddr_in6)
    // address information.
    struct sockaddr_storage client_addr;
    socklen_t sin_size = sizeof client_addr;

    int clientfd =
        accept(tcp_sock_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (clientfd == -1) {
        perror("ERROR: accept");
        return -1;
    }

    switch (client_addr.ss_family) {
        case AF_INET: /* IPv4 */
            inet_ntop(AF_INET,
                      &(((struct sockaddr_in *)(struct sockaddr *)&client_addr)
                            ->sin_addr),
                      conn->remote_addr,
                      sizeof conn->remote_addr / sizeof *conn->remote_addr);
            break;
        case AF_INET6: /* IPv6 */
            inet_ntop(AF_INET6,
                      &(((struct sockaddr_in6 *)(struct sockaddr *)&client_addr)
                            ->sin6_addr),
                      conn->remote_addr,
                      sizeof conn->remote_addr / sizeof *conn->remote_addr);
            break;
    }

    conn->clientfd = clientfd;

    return 0;
}

// Close the server socket if open.
static void close_tcp(void) {
    if (tcp_sock_fd != -1) {
        close(tcp_sock_fd);
    }
}

listener_t tcp_listener = {
    .listen = listen_tcp,
    .accept = accept_tcp,
    .close = close_tcp,
};

#ifndef LISTENER_H
#define LISTENER_H

// Exposes POSIX definitions like getaddrinfo(), struct addrinfo, etc.
#if defined(__unix__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L
#endif

#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct {
    char remote_addr[INET6_ADDRSTRLEN];
    int clientfd;
} connection_t;

typedef struct {
    void (*listen)(const char *host, const char *port, int backlog);
    int (*accept)(connection_t *conn);
    void (*close)(void);
} listener_t;

extern listener_t tcp_listener;

#endif  // LISTENER_H

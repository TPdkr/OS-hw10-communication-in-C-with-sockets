/*
These are functions from the slides
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include "tcp.h"
int tcp_connect(const char *host, const char *port){
    struct addrinfo hints, *ai_list, *ai;
    int rc, fd = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    rc = getaddrinfo(host, port, &hints, &ai_list);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }
    for (ai = ai_list; ai; ai = ai->ai_next) {
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) {
            switch (errno) {
            case EAFNOSUPPORT:
            case EPROTONOSUPPORT:
            continue;
            default:
            perror("socket");
            continue;
        }
        } else {
            if (connect(fd, ai->ai_addr, ai->ai_addrlen) == -1) {
                (void) close(fd);
                perror("connect");
                continue;
            }
        }
        break; /* we were successful, break out of the loop */
    }
    freeaddrinfo(ai_list);
    if (ai == NULL) {
        fprintf(stderr, "failed to connect to '%s' port '%s'\n", host, port);
        return -1;
    }
    return fd;
}

ssize_t tcp_write(int fd, const void *buf, size_t count){
    size_t nwritten = 0;
    int flags;
    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        return -1;
    }
    while (count > 0) {
        int r = write(fd, buf, count);
        if (r < 0 && errno == EINTR) {
            continue;
        }
        if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return nwritten;
        }
        if (r < 0) {
            return r;
        }
        if (r == 0) {
            return nwritten;
        }
        buf = (unsigned char *) buf + r;
        count -= r;
        nwritten += r;
        if ((flags & O_NONBLOCK) == 0) {
            return nwritten;
        }
    }
    return nwritten;
}

ssize_t tcp_read(int fd, void *buf, size_t count){
    size_t nread = 0;
    int flags;
    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        return -1;
    }
    while (count > 0) {
        int r = read(fd, buf, count);
        if (r < 0 && errno == EINTR) {
            continue;
        }
        if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return nread;
        }
        if (r < 0) {
            return r;
        }
        if (r == 0) {
            return nread;
        }
        buf = (unsigned char *) buf + r;
        count -= r;
        nread += r;
        if ((flags & O_NONBLOCK) == 0) {
            return nread;
        }
    }
    return nread;
}

int tcp_listen(const char *host, const char *port){
    struct addrinfo hints, *ai_list, *ai;
    int rc, fd = 0, on = 1;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    rc = getaddrinfo(host, port, &hints, &ai_list);
    if (rc) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }
    for (ai = ai_list; ai; ai = ai->ai_next) {
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) {
            continue;
        }
        #ifdef IPV6_V6ONLY
        if (ai->ai_family == AF_INET6) {
            (void) setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
        }
        #endif
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }
        (void) close(fd);
    }
    freeaddrinfo(ai_list);
    if (ai == NULL) {
        fprintf(stderr, "failed to bind to '%s' port %s\n", host, port);
        return -1;
    }
    if (listen(fd, 42) < 0) {
        perror("listen");
        (void) close(fd);
        return -1;
    }
    return fd;
}
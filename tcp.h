/*
FUNCTIONS FROM SLIDES OF OS
*/
#define HOST "127.0.0.1"
int tcp_connect(const char *host, const char *port);
ssize_t tcp_read(int fd, void *buf, size_t count);
ssize_t tcp_write(int fd, const void *buf, size_t count);
int tcp_listen(const char *host, const char *port);
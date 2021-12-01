#ifndef _SOCKIO_H_
#define _SOCKIO_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct socket_s {
  int sockfd;
  int domain;
  int type;
  int protocol;
  union socket_address_t {
    struct sockaddr_in *v4;
    struct sockaddr_in6 *v6;
  } address;
  char host[128];
  uint16_t port;
} socket_t;

bool socket_init();
socket_t *socket_create(const char *host, const uint16_t port, int type);
int socket_settimeout(socket_t *sock, uint64_t msec);
int socket_connect(socket_t *sock);
ssize_t socket_send(socket_t *sock, const void *data, size_t length);
ssize_t socket_recv(socket_t *sock, void *data, size_t lim);
ssize_t socket_peek(socket_t *sock, void *data, size_t len);
ssize_t socket_recvall(socket_t *sock, void *data, size_t len);
void socket_close(socket_t *sock);
void socket_deinit();
bool is_ipv6(const char *addr);

#endif

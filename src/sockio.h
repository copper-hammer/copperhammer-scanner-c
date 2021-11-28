#ifndef _SOCKIO_H_
#define _SOCKIO_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct socket_t {
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
} Socket;

bool socket_init();
struct socket_t *socket_create(const char *host, const uint16_t port, int type);
int socket_settimeout(struct socket_t *sock, uint64_t msec);
int socket_connect(struct socket_t *sock);
ssize_t socket_send(struct socket_t *sock, const void *data, size_t length);
ssize_t socket_recv(struct socket_t *sock, void *data, size_t lim);
ssize_t socket_peek(struct socket_t *sock, void *data, size_t len);
ssize_t socket_recvall(struct socket_t *sock, void *data, size_t len);
void socket_close(struct socket_t *sock);
void socket_deinit();
bool is_ipv6(const char *addr);

#endif

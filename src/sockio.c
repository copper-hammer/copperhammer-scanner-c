#include "sockio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

bool socket_init()
{
  // POSIX socket does not require initialization
  return true;
}

struct socket_t *socket_create(const char *host, const uint16_t port, int type)
{
  if (type != AF_INET && type != AF_INET6) return NULL;
  struct socket_t *sock = malloc(sizeof(struct socket_t));
  memset(sock, 0, sizeof(struct socket_t));
  sock->domain = type;
  sock->type = SOCK_STREAM;
  sock->protocol = 0;
  switch (sock->domain)
  {
    case AF_INET:
      sock->address.v4 = malloc(sizeof(struct sockaddr_in));
      sock->address.v4->sin_family = AF_INET;
      sock->address.v4->sin_port = htons(port);
      if (inet_pton(AF_INET, host, (void *)&sock->address.v4->sin_addr) != 1)
      {
        free(sock->address.v6);
        free(sock);
        return NULL;
      }
      break;

    case AF_INET6:
      sock->address.v6 = malloc(sizeof(struct sockaddr_in6));
      sock->address.v6->sin6_family = AF_INET6;
      sock->address.v6->sin6_port = htons(port);
      if(inet_pton(AF_INET6, host, (void *)&sock->address.v6->sin6_addr) != 1)
      {
        free(sock->address.v6);
        free(sock);
        return NULL;
      }
      break;
  }
  sock->sockfd = socket(sock->domain, sock->type, sock->protocol);
  return sock;
}

int socket_settimeout(struct socket_t *sock, uint64_t msec)
{
  struct timeval timeout;
  timeout.tv_sec = msec / 1000;
  timeout.tv_usec = (msec % 1000) * 1000;
  return setsockopt(sock->sockfd, SOL_SOCKET, SO_RCVTIMEO,
      (void *)&timeout, sizeof(struct timeval));
}

int socket_connect(struct socket_t *sock)
{
  switch (sock->domain)
  {
    case AF_INET:
      return connect(sock->sockfd,
          (struct sockaddr *)sock->address.v4, sizeof(struct sockaddr));
    case AF_INET6:
      return connect(sock->sockfd,
          (struct sockaddr *)sock->address.v6, sizeof(struct sockaddr_in6));
  }
  return -1;
}

ssize_t socket_send(struct socket_t *sock, const void *data, size_t length)
{
  return send(sock->sockfd, data, length, 0);
}

ssize_t socket_recv(struct socket_t *sock, void *data, size_t lim)
{
  return recv(sock->sockfd, data, lim, 0);
}

void socket_close(struct socket_t *sock)
{
  close(sock->sockfd);
  switch (sock->domain)
  {
    case AF_INET:
      free(sock->address.v4);
      break;
    case AF_INET6:
      free(sock->address.v6);
      break;
  }
  free(sock);
}

void socket_deinit()
{
}

bool is_ipv6(const char *addr)
{
  return (addr[0] == '[') && (addr[strlen(addr) - 1] == ']');
}


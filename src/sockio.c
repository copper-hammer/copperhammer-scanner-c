#include "logging.h"
#include "sockio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

bool socket_init()
{
  // POSIX socket does not require initialization
  // TODO: Windows (winsock2) support
  return true;
}

socket_t *socket_create(const char *host, const uint16_t port, int type)
{
  if (type != AF_INET && type != AF_INET6)
  {
    DBG(LOG_WARN, "Invalid socket type %d, expected AF_INET or AF_INET6", type);
    return NULL;
  }
  socket_t *sock = malloc(sizeof(socket_t));
  memset(sock, 0, sizeof(socket_t));
  sock->domain = type;
  sock->type = SOCK_STREAM;
  sock->protocol = 0;
  strncpy(sock->host, host, 128);
  sock->port = port;
  switch (sock->domain)
  {
    case AF_INET:
      sock->address.v4 = malloc(sizeof(struct sockaddr_in));
      sock->address.v4->sin_family = AF_INET;
      sock->address.v4->sin_port = htons(port);
      if (inet_pton(AF_INET, host, (void *)&sock->address.v4->sin_addr) != 1)
      {
        DBG(LOG_WARN, "Invalid IPv4 address: %s (%s)", host, strerror(errno));
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
        DBG(LOG_WARN, "Invalid IPv6 address: %s (%s)", host, strerror(errno));
        free(sock->address.v6);
        free(sock);
        return NULL;
      }
      break;
  }
  if ((sock->sockfd = socket(sock->domain, sock->type, sock->protocol)) == -1)
  {
    DBG(LOG_TRACE, "Socket creation failed: %s", strerror(errno));
    socket_close(sock);
    return NULL;
  }
  
  return sock;
}

int socket_settimeout(socket_t *sock, uint64_t msec)
{
  struct timeval timeout;
  timeout.tv_sec = msec / 1000;
  timeout.tv_usec = (msec % 1000) * 1000;
  return setsockopt(sock->sockfd, SOL_SOCKET, SO_RCVTIMEO,
      (void *)&timeout, sizeof(struct timeval));
}

int socket_connect(socket_t *sock)
{
  int result = -1;
  switch (sock->domain)
  {
    case AF_INET:
      result = connect(sock->sockfd,
          (struct sockaddr *)sock->address.v4, sizeof(struct sockaddr));
      break;
    case AF_INET6:
      result = connect(sock->sockfd,
          (struct sockaddr *)sock->address.v6, sizeof(struct sockaddr_in6));
      break;
  }
  if (result != 0)
    DBG(LOG_WARN, "%s:%d: %s", sock->host, sock->port, strerror(errno));
  return result;
}

ssize_t socket_send(socket_t *sock, const void *data, size_t length)
{
  DBG(LOG_TRACE, "Sending %zd bytes at %p to %d:", length, data, sock->sockfd);
  DHEX(LOG_TRACE, data, length);
  ssize_t res = send(sock->sockfd, data, length, 0);
  if (res < 0)
    DBG(LOG_WARN, "Send failed: %s", strerror(errno));
  return res;
}

ssize_t __socket_recv(socket_t *sock, void *data, size_t lim, int flags)
{
  DBG(LOG_TRACE, "recv(sock:%d, data:%p, lim:%zu, flags:%08x) called",
      sock->sockfd, data, lim, flags);
  ssize_t res = recv(sock->sockfd, data, lim, flags);
  DBG(LOG_TRACE, "recv(sock:%d, data:%p, lim:%zu, flags:%08x): %zd",
      sock->sockfd, data, lim, flags, res);
  if (res < 0)
  {
    DBG(LOG_WARN, "Recv failed: %s", strerror(errno));
  }
  else
  {
    DHEX(LOG_TRACE, data, res);
  }
  return res;
}

ssize_t socket_recv(socket_t *sock, void *data, size_t lim)
{
  return __socket_recv(sock, data, lim, 0);
}

ssize_t socket_recvall(socket_t *sock, void *data, size_t len)
{
  return __socket_recv(sock, data, len, MSG_WAITALL);
}

ssize_t socket_peek(socket_t *sock, void *data, size_t len)
{
  return __socket_recv(sock, data, len, MSG_PEEK | MSG_WAITALL);
}

void socket_close(socket_t *sock)
{
  if (sock->sockfd >= 0)
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


#include "mcping.h"
#include "mcproto.h"
#include "sockio.h"
#include <string.h>

ssize_t mc_ping(const char *host, const uint16_t port, void *buffer, size_t lim)
{
  struct socket_t *sock = socket_create(host, port, is_ipv6(host) ? AF_INET6 : AF_INET);
  socket_settimeout(sock, 500);
  ssize_t res;
  if ((res = socket_connect(sock)) != 0)
  {
    socket_close(sock);
    return res;
  }
  
  res = mc_ping_sock(host, port, (const struct socket_t *)sock, buffer, lim);

  socket_close(sock);
  return res;
}

ssize_t mc_ping_sock(const char *host, const uint16_t port, const struct socket_t *sock, void *buffer, size_t lim)
{
  void *packet = malloc(strlen(host) + 10);
  ssize_t pkt_size = mc_ping_make_packet(host, port, -1, packet, strlen(host) + 10);
  socket_send((struct socket_t *)sock, packet, pkt_size);
  socket_recv((struct socket_t *)sock, buffer, lim);
  int32_t length;
  void *tp;
  if ((tp = mc_read_varint(buffer, &length)) == NULL)
    return -1;
  if (length < 0 || length > 32767)
    return -1;
  uint8_t pkt_type;
  tp = mc_read_ubyte(tp, &pkt_type);
  if (pkt_type != 0x00)
    return -1;
  int32_t result_len;
  tp = mc_read_string(tp, buffer, &result_len, lim);
  if (result_len < 0 || result_len > 32767)
    return -1;
  if (result_len < lim)
    memset(buffer + result_len, 0, lim - result_len);
  return result_len;
}

ssize_t mc_ping_make_packet(const char *host, uint16_t port, int32_t ver, void *buffer, size_t lim)
{
  void *tp = buffer;
  tp += mc_write_ubyte(tp, 0x00); // PACKET_HANDSHAKE
  tp += mc_write_varint(tp, ver);
  tp += mc_write_string(tp, (char *)host, strlen(host));
  tp += mc_write_ushort(tp, port);
  tp += mc_write_varint(tp, 1); // NEXT_STATE=STATUS
  size_t packet_size = tp - buffer;
  memmove(buffer + mc_size_varnum(packet_size), buffer, packet_size);
  tp = buffer + mc_write_varint(buffer, packet_size) + packet_size;
  tp += mc_write_varint(tp, 1);
  tp += mc_write_ubyte(tp, 0);
  return tp - buffer;
}


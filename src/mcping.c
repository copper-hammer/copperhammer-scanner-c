#include "logging.h"
#include "mcping.h"
#include "mcproto.h"
#include "sockio.h"
#include <string.h>


ssize_t mcp_send_ping(socket_t *s,
    const char *host, const uint16_t port, int32_t ver) {
  uint8_t buffer[1024];
  ssize_t pkt_size = mcp_ping_make_packet(host, port, ver, buffer, 1024);
  return socket_send(s, buffer, pkt_size);
}

ssize_t mcp_read_pong(socket_t *sock, void *buffer, size_t lim)
{
  ssize_t len;
#if __APPLE__
  size_t lim_backup_macos_sucks = lim;
#endif
  if (mc_sread_varint(sock, (int32_t *)&len) <= 0)
    return -1;
#if __APPLE__
  lim = lim_backup_macos_sucks;
#endif
  if (len <= 0 || len > 32768)
    return -1;
  ssize_t n_bytes = ((ssize_t)lim < len) ? (ssize_t)lim : len;
  ssize_t r;
  DBG(LOG_TRACE, "pkt %zd long, %zd limit", len, lim);
  if ((r = mc_sread_raw(sock, buffer, n_bytes)) <= 0)
  {
    DBG(LOG_WARN, "Failed to read packet body");
    return -1;
  }
  DBG(LOG_TRACE, "pkt received: %zd/%zd", r, n_bytes);
  DHEX(LOG_TRACE, buffer, n_bytes);
  uint8_t *tp = buffer;
  uint8_t pkt_type;
  tp = mc_read_ubyte(tp, &pkt_type);
  DBG(LOG_TRACE, "pkt type: %02x", pkt_type);
  if (pkt_type != 0x00)
    return -1;
  mc_read_string(tp, buffer, (int32_t *)&len, lim);
  if (len < (ssize_t)lim)
    memset((uint8_t *)buffer + len, 0, lim - len);
  return len;
}

ssize_t mcp_ping_make_packet(const char *host, uint16_t port,
    int32_t ver, void *buf, size_t lim)
{
  (void)lim; // TODO: keep an eye on limits
  uint8_t *tp = buf;
  tp += mc_write_ubyte(tp, 0x00); // PACKET_HANDSHAKE
  tp += mc_write_varint(tp, ver);
  tp += mc_write_string(tp, (char *)host, strlen(host));
  tp += mc_write_ushort(tp, port);
  tp += mc_write_varint(tp, 1); // NEXT_STATE=STATUS
  size_t packet_size = tp - (uint8_t *)buf;
  memmove((uint8_t *)buf + mc_size_varnum(packet_size), buf, packet_size);
  tp = ((uint8_t *)buf) + mc_write_varint(buf, packet_size) + packet_size;
  tp += mc_write_varint(tp, 1);
  tp += mc_write_ubyte(tp, 0);
  return tp - (uint8_t *)buf;
}


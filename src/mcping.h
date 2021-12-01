#ifndef _MCPING_H_
#define _MCPING_H_
#include <stdint.h>
#include <stdlib.h>
#include "sockio.h"

ssize_t mcp_send_ping(socket_t *s, const char *host, const uint16_t port,
    int32_t ver);
ssize_t mcp_read_pong(socket_t *s, void *buffer, size_t lim);
ssize_t mcp_ping_make_packet(const char *host, uint16_t port,
    int32_t ver, void *buf, size_t lim);

#endif

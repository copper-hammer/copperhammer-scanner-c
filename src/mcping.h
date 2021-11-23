#ifndef _MCPING_H_
#define _MCPING_H_
#include <stdint.h>
#include <stdlib.h>
#include "sockio.h"

ssize_t mc_ping(const char *host, const uint16_t port, void *buffer, size_t lim);
ssize_t mc_ping_sock(const char *host, const uint16_t port, const struct socket_t *sock, void *buffer, size_t lim);
ssize_t mc_ping_make_packet(const char *host, uint16_t port, int32_t ver, void *buffer, size_t lim);

#endif

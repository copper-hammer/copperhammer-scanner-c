#ifndef _MCPROTO_H_
#define _MCPROTO_H_
#include "logging.h"
#include "sockio.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


size_t mc_size_varnum(int64_t val);

// Buffer-bound functions
uint8_t *mc_read_boolean(uint8_t *buf, bool *val);
uint8_t *mc_read_byte(uint8_t *buf, int8_t *val);
uint8_t *mc_read_ubyte(uint8_t *buf, uint8_t *val);
uint8_t *mc_read_short(uint8_t *buf, int16_t *val);
uint8_t *mc_read_ushort(uint8_t *buf, uint16_t *val);
uint8_t *mc_read_int(uint8_t *buf, int32_t *val);
uint8_t *mc_read_long(uint8_t *buf, int64_t *val);
uint8_t *mc_read_float(uint8_t *buf, float *val);
uint8_t *mc_read_double(uint8_t *buf, double *val);
uint8_t *mc_read_string(uint8_t *buf, char *val, int32_t *len, ssize_t lim);
uint8_t *mc_read_raw(uint8_t *buf, uint8_t *out, ssize_t len);
uint8_t *mc_read_varint(uint8_t *buf, int32_t *val);
uint8_t *mc_read_varlong(uint8_t *buf, int64_t *val);
ssize_t mc_write_boolean(uint8_t *buf, bool val);
ssize_t mc_write_byte(uint8_t *buf, int8_t val);
ssize_t mc_write_ubyte(uint8_t *buf, uint8_t val);
ssize_t mc_write_short(uint8_t *buf, int16_t val);
ssize_t mc_write_ushort(uint8_t *buf, uint16_t val);
ssize_t mc_write_int(uint8_t *buf, int32_t val);
ssize_t mc_write_long(uint8_t *buf, int64_t val);
ssize_t mc_write_float(uint8_t *buf, float val);
ssize_t mc_write_double(uint8_t *buf, double val);
ssize_t mc_write_string(uint8_t *buf, char *val, int32_t len);
ssize_t mc_write_raw(uint8_t *buf, uint8_t *val, int32_t len);
ssize_t mc_write_varint(uint8_t *buf, int32_t val);
ssize_t mc_write_varlong(uint8_t *buf, int64_t val);

// Socket-bound functions
ssize_t mc_sread_boolean(socket_t *so, bool *val);
ssize_t mc_sread_byte(socket_t *so, int8_t *val);
ssize_t mc_sread_ubyte(socket_t *so, uint8_t *val);
ssize_t mc_sread_short(socket_t *so, int16_t *val);
ssize_t mc_sread_ushort(socket_t *so, uint16_t *val);
ssize_t mc_sread_int(socket_t *so, int32_t *val);
ssize_t mc_sread_long(socket_t *so, int64_t *val);
ssize_t mc_sread_float(socket_t *so, float *val);
ssize_t mc_sread_double(socket_t *so, double *val);
ssize_t mc_sread_string(socket_t *so, char *val, int32_t *len, ssize_t lim);
ssize_t mc_sread_raw(socket_t *so, char *val, ssize_t len);
ssize_t mc_sread_varint(socket_t *so, int32_t *val);
ssize_t mc_sread_varlong(socket_t *so, int64_t *val);
ssize_t mc_swrite_boolean(socket_t *so, bool val);
ssize_t mc_swrite_byte(socket_t *so, int8_t val);
ssize_t mc_swrite_ubyte(socket_t *so, uint8_t val);
ssize_t mc_swrite_short(socket_t *so, int16_t val);
ssize_t mc_swrite_ushort(socket_t *so, uint16_t val);
ssize_t mc_swrite_int(socket_t *so, int32_t val);
ssize_t mc_swrite_long(socket_t *so, int64_t val);
ssize_t mc_swrite_float(socket_t *so, float val);
ssize_t mc_swrite_double(socket_t *so, double val);
ssize_t mc_swrite_string(socket_t *so, char *val, int32_t length);
ssize_t mc_swrite_raw(socket_t *so, uint8_t *val, ssize_t len);
ssize_t mc_swrite_varint(socket_t *so, int32_t val);
ssize_t mc_swrite_varlong(socket_t *so, int64_t val);

#endif

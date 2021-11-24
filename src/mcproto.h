#ifndef _MCPROTO_H_
#define _MCPROTO_H_
#include "logging.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

ssize_t mc_write_boolean(uint8_t *buf, bool value);
ssize_t mc_write_byte(uint8_t *buf, int8_t value);
ssize_t mc_write_ubyte(uint8_t *buf, uint8_t value);
ssize_t mc_write_short(uint8_t *buf, int16_t value);
ssize_t mc_write_ushort(uint8_t *buf, uint16_t value);
ssize_t mc_write_int(uint8_t *buf, int32_t value);
ssize_t mc_write_long(uint8_t *buf, int64_t value);
ssize_t mc_write_float(uint8_t *buf, float value);
ssize_t mc_write_double(uint8_t *buf, double value);
ssize_t mc_write_string(uint8_t *buf, char *value, int32_t length);
ssize_t mc_write_raw(uint8_t *buf, uint8_t *value, int32_t length);
ssize_t mc_write_varint(uint8_t *buf, int32_t value);
ssize_t mc_write_varlong(uint8_t *buf, int64_t value);

size_t mc_size_varnum(int32_t value);

uint8_t *mc_read_boolean(uint8_t *buf, bool *value);
uint8_t *mc_read_byte(uint8_t *buf, int8_t *value);
uint8_t *mc_read_ubyte(uint8_t *buf, uint8_t *value);
uint8_t *mc_read_short(uint8_t *buf, int16_t *value);
uint8_t *mc_read_ushort(uint8_t *buf, uint16_t *value);
uint8_t *mc_read_int(uint8_t *buf, int32_t *value);
uint8_t *mc_read_long(uint8_t *buf, int64_t *value);
uint8_t *mc_read_float(uint8_t *buf, float *value);
uint8_t *mc_read_double(uint8_t *buf, double *value);
uint8_t *mc_read_string(uint8_t *buf, char *value, int32_t *length, ssize_t lim);
uint8_t *mc_read_raw(uint8_t *buf, uint8_t *out, ssize_t length);
uint8_t *mc_read_varint(uint8_t *buf, int32_t *value);
uint8_t *mc_read_varlong(uint8_t *buf, int64_t *value);
#endif

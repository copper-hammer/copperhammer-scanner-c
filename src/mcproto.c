#include "logging.h"
#include "mcproto.h"
#include "sockio.h"
#include <endian.h>
#include <math.h>

size_t mc_size_varnum(int64_t value)
{
  return ceilf(log2f((float)value) / 7.0f);
}

// Buffer-bound read functions
ssize_t mc_write_boolean(uint8_t *buf, bool value)
{
  DBG(LOG_TRACE, "Writing bool:%d to %p", value, buf);
  (*buf++) = value ? 1 : 0;
  return 1;
}

ssize_t mc_write_byte(uint8_t *buf, int8_t value)
{
  DBG(LOG_TRACE, "Writing byte:%02x to %p", value, buf);
  (*buf++) = *(uint8_t *)&value;
  return 1;
}

ssize_t mc_write_ubyte(uint8_t *buf, uint8_t value)
{
  DBG(LOG_TRACE, "Writing ubyte:%02x to %p", value, buf);
  (*buf++) = value;
  return 1;
}

ssize_t mc_write_short(uint8_t *buf, int16_t value)
{
  DBG(LOG_TRACE, "Writing short:%d to %p", value, buf);
  uint16_t val = *(uint16_t *)&value;
  (*buf++) = (val & 0xff00) >> 8;
  (*buf++) = val & 0xff;
  return 2;
}

ssize_t mc_write_ushort(uint8_t *buf, uint16_t value)
{
  DBG(LOG_TRACE, "Writing ushort:%d to %p", value, buf);
  (*buf++) = (value & 0xff00) >> 8;
  (*buf++) = value & 0xff;
  return 2;
}

ssize_t mc_write_int(uint8_t *buf, int32_t value)
{
  DBG(LOG_TRACE, "Writing int32:%d to %p", value, buf);
  uint32_t val = htobe32(value);
  memcpy(buf, (const void *)&val, 4);
  return 4;
}

ssize_t mc_write_long(uint8_t *buf, int64_t value)
{
  DBG(LOG_TRACE, "Writing int64:%d to %p", value, buf);
  uint64_t val = htobe64(value);
  memcpy(buf, (const void *)&val, 8);
  return 8;
}

ssize_t mc_write_float(uint8_t *buf, float value)
{
  DBG(LOG_TRACE, "Writing float:%f to %p", value, buf);
  memcpy(buf, (const void *)&value, 4);
  return 4;
}

ssize_t mc_write_double(uint8_t *buf, double value)
{
  DBG(LOG_TRACE, "Writing double:%lf to %p", value, buf);
  memcpy(buf, (const void *)&value, 8);
  return 8;
}

ssize_t mc_write_string(uint8_t *buf, char *value, int32_t length)
{
  DBG(LOG_TRACE, "Writing string(%d):%s to %p", length, value, buf);
  ssize_t varint_len = mc_write_varint(buf, length);
  if (varint_len < 0)
    return -1;
  buf += varint_len;
  memcpy((void *)buf, value, length);
  return varint_len + length;
}

ssize_t mc_write_raw(uint8_t *buf, uint8_t *value, int32_t length)
{
  DBG(LOG_TRACE, "Writing raw(%d) to %p", length, buf);
  memcpy(buf, value, length);
  return length;
}

ssize_t __mc_write_varnum(uint8_t *buf, int64_t value, bool is_long)
{
  uint64_t v = *(uint64_t *)&value;
  uint64_t mask = is_long ? 0xFFFFFFFFFFFFFF80 : 0xFFFFFF80;
  if (!is_long) v &= 0xFFFFFFFF;
  ssize_t bytes = 0;
  while (1) {
    if ((v & mask) == 0) {
      (*buf++) = v & 0xFF;
      return ++bytes;
    }
    
    (*buf++) = (v & 0x7f) | 0x80;
    v >>= 7;
    bytes++;
  }
  return bytes;
}

ssize_t mc_write_varint(uint8_t *buf, int32_t value)
{
  DBG(LOG_TRACE, "Writing varint:%d to %p", value, buf);
  return __mc_write_varnum(buf, value, false);
}

ssize_t mc_write_varlong(uint8_t *buf, int64_t value)
{
  DBG(LOG_TRACE, "Writing varlong:%d to %p", value, buf);
  return __mc_write_varnum(buf, value, true);
}

// Buffer-bound write functions
uint8_t *mc_read_boolean(uint8_t *buf, bool *value)
{
  *value = (*buf++) ? true : false;
  return buf;
}

uint8_t *mc_read_byte(uint8_t *buf, int8_t *value)
{
  *value = *buf++;
  return buf;
}

uint8_t *mc_read_ubyte(uint8_t *buf, uint8_t *value)
{
  *value = *buf++;
  return buf;
}

uint8_t *mc_read_short(uint8_t *buf, int16_t *value)
{
  *value = ((*buf++) << 8);
  *value |= (*buf++);
  return buf;
}

uint8_t *mc_read_ushort(uint8_t *buf, uint16_t *value)
{
  *value = ((*buf++) << 8);
  *value |= (*buf++);
  return buf;
}

uint8_t *mc_read_int(uint8_t *buf, int32_t *value)
{
  for (int i = 0; i < 4; i++)
    *value = ((*value) << 8) | (*buf++);
  return buf;
}

uint8_t *mc_read_long(uint8_t *buf, int64_t *value)
{
  for (int i = 0; i < 8; i++)
    *value = ((*value) << 8) | (*buf++);
  return buf;
}

uint8_t *mc_read_float(uint8_t *buf, float *value)
{
  uint32_t val = 0;
  for (int i = 0; i < 4; i++)
    val = (val << 8) | (*buf++);
  *value = *(float*)&val;
  return buf;
}

uint8_t *mc_read_double(uint8_t *buf, double *value)
{
  uint64_t val = 0;
  for (int i = 0; i < 8; i++)
    val = (val << 8) | (*buf++);
  *value = *(double *)&val;
  return buf;
}

uint8_t *mc_read_string(uint8_t *buf, char *value, int32_t *length, ssize_t lim)
{
  buf = mc_read_varint(buf, length);
  for (int32_t i = 0; i < *length; i++)
  {
    if (i < lim)
      (*value++) = (*buf++);
    else
      buf++;
  }
  return buf;
}

uint8_t *mc_read_raw(uint8_t *buf, uint8_t *out, ssize_t length)
{
  memmove(out, buf, length);
  return buf + length;
}

uint8_t *__mc_read_varnum(uint8_t *buf, void *ptr, size_t len)
{
  uint64_t *value = ptr;
  *value = 0;
  int offset = 0;
  uint8_t currentByte;
  do
  {
    if (offset >= len) {
      *value = 0;
      return NULL;
    }
    currentByte = (*buf++);
    *value |= (currentByte & 0x7f) << (offset * 7);
    offset++;
  } while ((currentByte & 0x80) != 0);
  return buf;
}

uint8_t *mc_read_varint(uint8_t *buf, int32_t *value)
{
  return __mc_read_varnum(buf, value, 5);
}

uint8_t *mc_read_varlong(uint8_t *buf, int64_t *value)
{
  return __mc_read_varnum(buf, value, 10);
}

// Socket-bound read functions
ssize_t mc_sread_boolean(struct socket_t *so, bool *val)
{
  return socket_recvall(so, val, 1);
}

ssize_t mc_sread_byte(struct socket_t *so, int8_t *val)
{
  return socket_recvall(so, val, 1);
}

ssize_t mc_sread_ubyte(struct socket_t *so, uint8_t *val)
{
  return socket_recvall(so, val, 1);
}

ssize_t mc_sread_short(struct socket_t *so, int16_t *val)
{
  ssize_t r = socket_recvall(so, val, 2);
  *val = be16toh(*val);
  return r;
}

ssize_t mc_sread_ushort(struct socket_t *so, uint16_t *val)
{
  ssize_t r = socket_recvall(so, val, 2);
  *val = be16toh(*val);
  return r;
}

ssize_t mc_sread_int(struct socket_t *so, int32_t *val)
{
  ssize_t r = socket_recvall(so, val, 4);
  *val = be32toh(*val);
  return r;
}

ssize_t mc_sread_long(struct socket_t *so, int64_t *val)
{
  ssize_t r = socket_recvall(so, val, 8);
  *val = be64toh(*val);
  return r;
}

ssize_t mc_sread_float(struct socket_t *so, float *val)
{
  return socket_recvall(so, val, 4);
}

ssize_t mc_sread_double(struct socket_t *so, double *val)
{
  return socket_recvall(so, val, 8);
}
ssize_t mc_sread_string(struct socket_t *so, char *val, int32_t *len, ssize_t lim)
{
  ssize_t lr = mc_sread_varint(so, len);
  if (lr <= 0) return lr;
  ssize_t sr = socket_recvall(so, val, *len);
  if (*len < lim)
  {
    ssize_t rr;
    void *d = malloc(lim - *len);
    if ((rr = socket_recvall(so, d, lim - *val)) <= 0)
      return rr;
    free(d);
    sr += rr;
  }
  return sr + lr;
}

ssize_t mc_sread_raw(struct socket_t *so, char *val, ssize_t len)
{
  return socket_recvall(so, val, len);
}

ssize_t __mc_sread_varnum(struct socket_t *so, void *ptr, size_t len)
{
  uint64_t *val = ptr;
  *val = 0;
  int offset = 0;
  uint8_t buf[8];
  ssize_t r;
  if ((r = socket_peek(so, buf, 5)) <= 0)
    return r;
  do
  {
    if (offset >= len)
    {
      DBG(LOG_WARN, "varnum out of bounds (%zd/%zd)", offset, len);
      *val = 0;
      return -1;
    }
    DBG(LOG_TRACE, "varnum: %d ~ %02x (i=%d)", *val, buf[offset], offset);
    *val |= (buf[offset] & 0x7f) << (offset * 7);
    offset++;
  }
  while ((buf[offset - 1] & 0x80) != 0);
  socket_recv(so, buf, offset);
  return offset;
}

ssize_t mc_sread_varint(struct socket_t *so, int32_t *val)
{
  return __mc_sread_varnum(so, val, 5);
}

ssize_t mc_sread_varlong(struct socket_t *so, int64_t *val)
{
  return __mc_sread_varnum(so, val, 10);
}

// Socket-bound write functions
ssize_t mc_swrite_boolean(struct socket_t *so, bool val)
{
  return socket_send(so, &val, 1);
}

ssize_t mc_swrite_byte(struct socket_t *so, int8_t val)
{
  return socket_send(so, &val, 1);
}

ssize_t mc_swrite_ubyte(struct socket_t *so, uint8_t val)
{
  return socket_send(so, &val, 1);
}

ssize_t mc_swrite_short(struct socket_t *so, int16_t val)
{
  val = htobe16(val);
  return socket_send(so, &val, 2);
}

ssize_t mc_swrite_ushort(struct socket_t *so, uint16_t val)
{
  val = htobe16(val);
  return socket_send(so, &val, 2);
}

ssize_t mc_swrite_int(struct socket_t *so, int32_t val)
{
  val = htobe32(val);
  return socket_send(so, &val, 4);
}

ssize_t mc_swrite_long(struct socket_t *so, int64_t val)
{
  val = htobe64(val);
  return socket_send(so, &val, 8);
}

ssize_t mc_swrite_float(struct socket_t *so, float val)
{
  return socket_send(so, &val, 4);
}

ssize_t mc_swrite_double(struct socket_t *so, double val)
{
  return socket_send(so, &val, 8);
}

ssize_t mc_swrite_string(struct socket_t *so, char *val, int32_t length)
{
  ssize_t r;
  if ((r = mc_swrite_varint(so, length)) <= 0)
    return r;
  ssize_t o;
  if ((o = mc_swrite_raw(so, (uint8_t *)val, length)) <= 0)
    return o;
  return r + o;
}

ssize_t mc_swrite_raw(struct socket_t *so, uint8_t *val, ssize_t len)
{
  return socket_send(so, val, len);
}

ssize_t mc_swrite_varint(struct socket_t *so, int32_t val)
{
  uint8_t buf[8]; size_t len = mc_size_varnum(val);
  mc_write_varint(buf, val);
  return socket_send(so, buf, len);
}

ssize_t mc_swrite_varlong(struct socket_t *so, int64_t val)
{
  uint8_t buf[8]; size_t len = mc_size_varnum(val);
  mc_write_varlong(buf, val);
  return socket_send(so, buf, len);
}


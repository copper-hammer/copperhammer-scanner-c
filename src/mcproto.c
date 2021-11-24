#include "logging.h"
#include "mcproto.h"
#include <math.h>

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
  uint32_t val = *(uint32_t *)&value;
  (*buf++) = (val & 0xff000000) >> 24;
  (*buf++) = (val & 0x00ff0000) >> 16;
  (*buf++) = (val & 0x0000ff00) >> 8;
  (*buf++) = (val & 0x000000ff);
  return 4;
}

ssize_t mc_write_long(uint8_t *buf, int64_t value)
{
  DBG(LOG_TRACE, "Writing int64:%d to %p", value, buf);
  uint64_t val = *(uint64_t *)&value;
  (*buf++) = (val & 0xff00000000000000) >> 56;
  (*buf++) = (val & 0x00ff000000000000) >> 48;
  (*buf++) = (val & 0x0000ff0000000000) >> 40;
  (*buf++) = (val & 0x000000ff00000000) >> 32;
  (*buf++) = (val & 0x00000000ff000000) >> 24;
  (*buf++) = (val & 0x0000000000ff0000) >> 16;
  (*buf++) = (val & 0x000000000000ff00) >> 8;
  (*buf++) = (val & 0x00000000000000ff) >> 0;
  return 8;
}

ssize_t mc_write_float(uint8_t *buf, float value)
{
  DBG(LOG_TRACE, "Writing float:%f to %p", value, buf);
  uint32_t val = *(uint32_t *)&value;
  (*buf++) = (val & 0xff000000) >> 24;
  (*buf++) = (val & 0x00ff0000) >> 16;
  (*buf++) = (val & 0x0000ff00) >> 8;
  (*buf++) = (val & 0x000000ff);
  return 4;
}

ssize_t mc_write_double(uint8_t *buf, double value)
{
  DBG(LOG_TRACE, "Writing double:%lf to %p", value, buf);
  uint64_t val = *(uint64_t *)&value;
  (*buf++) = (val & 0xff00000000000000) >> 56;
  (*buf++) = (val & 0x00ff000000000000) >> 48;
  (*buf++) = (val & 0x0000ff0000000000) >> 40;
  (*buf++) = (val & 0x000000ff00000000) >> 32;
  (*buf++) = (val & 0x00000000ff000000) >> 24;
  (*buf++) = (val & 0x0000000000ff0000) >> 16;
  (*buf++) = (val & 0x000000000000ff00) >> 8;
  (*buf++) = (val & 0x00000000000000ff) >> 0;
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

ssize_t mc_write_varint(uint8_t *buf, int32_t value)
{
  DBG(LOG_TRACE, "Writing varint:%d to %p", value, buf);
  uint32_t v = *(uint32_t*)&value;
  ssize_t bytes = 0;

  while (1) {
    if ((v & 0xFFFFFF80) == 0) {
      (*buf++) = v & 0xFF;
      return ++bytes;
    }
    
    (*buf++) = (v & 0x7f) | 0x80;
    v >>= 7;
    bytes++;
  }
  return bytes;
}

ssize_t mc_write_varlong(uint8_t *buf, int64_t value)
{
  DBG(LOG_TRACE, "Writing varlong:%d to %p", value, buf);
  uint32_t v = *(uint32_t*)&value;
  ssize_t bytes = 0;

  while (1) {
    if ((v & 0xFFFFFFFFFFFFFF80) == 0) {
      (*buf++) = v & 0xFF;
      return ++bytes;
    }
    
    (*buf++) = (v & 0x7f) | 0x80;
    v >>= 7;
    bytes++;
  }
  return bytes;
}

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

uint8_t *mc_read_varint(uint8_t *buf, int32_t *value)
{
  *value = 0;
  int bitOffset = 0;
  uint8_t currentByte;
  do {
    if (bitOffset >= 5) {
      *value = 0;
      return NULL;
    }
    currentByte = (*buf++);
    *value |= (currentByte & 0x7f) << (bitOffset * 7);
    bitOffset++;
  } while ((currentByte & 0x80) != 0);
  return buf;
}

uint8_t *mc_read_varlong(uint8_t *buf, int64_t *value)
{
  *value = 0;
  int bitOffset = 0;
  uint8_t currentByte;
  do {
    if (bitOffset >= 10) {
      *value = 0;
      return NULL;
    }
    currentByte = (*buf++);
    *value |= (currentByte & 0x7f) << (bitOffset * 7);
    bitOffset++;
  } while ((currentByte & 0x80) != 0);
  return buf;
}

size_t mc_size_varnum(int32_t value)
{
  return ceilf(log2f((float)value) / 7.0f);
}


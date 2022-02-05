#ifndef _LOGGING_H_
#define _LOGGING_H_
#include <stdio.h>

#define DBG(level, ...) \
  if (level <= loglevel) debug_print(__FILE__, __LINE__, level, __VA_ARGS__)
#define DHEX(level, ptr, len) \
  if (level <= loglevel) debug_hexdump(__FILE__, __LINE__, level, ptr, len)
#define DBGK(level, ...) if (level <= loglevel) fprintf(stderr, __VA_ARGS__)

typedef enum {
  LOG_NONE = 0,
  LOG_ERROR = 1,
  LOG_WARN = 2,
  LOG_INFO = 3,
  LOG_DEBUG = 4,
  LOG_TRACE = 5,
  LOG_ENDL = 6
} loglevel_n;

extern loglevel_n loglevel;

void debug_print(char *file, int ln, loglevel_n lvl, const char *fmt, ...);
void debug_hexdump(char *file, int ln, loglevel_n lvl, const void *p, size_t l);
const char *get_loglevel_name(loglevel_n lvl);

#endif

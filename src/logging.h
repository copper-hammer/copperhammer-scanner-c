#ifndef _LOGGING_H_
#define _LOGGING_H_
#include <stdio.h>
#define DBG(level, ...) if (level <= loglevel) debug_print(__FILE__, __LINE__, level, __VA_ARGS__)
#define DBGK(level, ...) if (level <= loglevel) fprintf(stderr, __VA_ARGS__)

#define loglevel (*__get_loglevel_addr())

typedef enum {
  LOG_NONE = 0,
  LOG_ERROR = 1,
  LOG_WARN = 2,
  LOG_INFO = 3,
  LOG_DEBUG = 4,
  LOG_TRACE = 5,
  LOG_ENDL = 6
} loglevel_n;

extern loglevel_n *__get_loglevel_addr(void);
void debug_print(char *filename, int line, loglevel_n level, const char *fmt, ...);
const char *get_loglevel_name(loglevel_n level);

#endif

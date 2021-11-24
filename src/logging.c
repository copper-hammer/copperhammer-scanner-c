#include "logging.h"
#include <stdarg.h>

const char *loglevel_names[7] = {
  "QUIET", "ERROR", "WARN", "INFO", "DEBUG", "TRACE", "TRACE+"
};

void debug_print(char *filename, int line, loglevel_n level, const char *fmt, ...)
{
  va_list args;
  char buffer[2048];
  va_start(args, fmt);
  vsnprintf(buffer, 2048, fmt, args);
  fprintf(stderr, "%-5s %s:%d: %s\n", get_loglevel_name(level), filename, line, buffer);
  va_end(args);
}

const char *get_loglevel_name(loglevel_n level)
{
  if (level >= LOG_ENDL) return loglevel_names[LOG_ENDL];
  return loglevel_names[level];
}

extern loglevel_n *__get_loglevel_addr(void)
{
  static loglevel_n level = LOG_NONE;
  return &level;
}
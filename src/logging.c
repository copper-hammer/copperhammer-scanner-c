#include "logging.h"
#include "utils.h"
#include <stdarg.h>
#include <pthread.h>

const char *loglevel_names[7] = {
  "QUIET", "ERROR", "WARN", "INFO", "DEBUG", "TRACE", "TRACE+"
};

loglevel_n loglevel = LOG_NONE;

void debug_print(char *file, int line, loglevel_n lvl, const char *fmt, ...)
{
  va_list args;
  char buf[2048];
  va_start(args, fmt);
  vsnprintf(buf, 2048, fmt, args);
  fprintf(stderr, "%-5s [T%08lx] %s:%d: %s\n",
      get_loglevel_name(lvl), pthread_self(), file, line, buf);
  va_end(args);
}

void debug_hexdump(char *f, int l, loglevel_n lvl, const void *ptr, size_t len)
{
  char buf[1024];
  snprintf(buf, 1024, "%-5s %s:%d: ", get_loglevel_name(lvl), f, l);
  hexdumpfp(ptr, len, stderr, buf);
}

const char *get_loglevel_name(loglevel_n level)
{
  if (level >= LOG_ENDL) return loglevel_names[LOG_ENDL];
  return loglevel_names[level];
}



#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

void hexdump(void *data, size_t size)
{
  hexdumpf(data, size, stdout);
}

void hexdump_relative(void *data, size_t size)
{
  hexdumpf_relative(data, size, stdout);
}

void hexdumpf(void *data, size_t size, FILE *file)
{
  hexdump_impl(data, size, 0, file);
}

void hexdumpf_relative(void *data, size_t size, FILE *file)
{
  hexdump_impl(data, size, 1, file);
}

void hexdump_impl(void *data, size_t size, int type, FILE *file)
{
  for (size_t i = 0; i < size; i += 16)
  {
    if (type == 0)
      fprintf(file, "%p\t", (void *)((uint8_t *)data + i));
    else
      fprintf(file, "%08lx\t", i);
    int k;
    for (k = 0; k < 16 && (k + i) < size; k++)
    {
      fprintf(file, "%02x", ((uint8_t *)data)[k + i]);
      if (k % 4 == 3) fprintf(file, " ");
    }
    for (; k < 16; k++)
    {
      fprintf(file, "--");
      if (k % 4 == 3) fprintf(file, " ");
    }
    fprintf(file, "\t|");
    for (k = 0; k < 16 && (k + i) < size; k++)
    {
      if (isprint(((char *)data)[k + i]))
        fprintf(file, "%c", ((char *)data)[k + i]);
      else
        fprintf(file, ".");
    }
    fprintf(file, "|\n");
  }
}

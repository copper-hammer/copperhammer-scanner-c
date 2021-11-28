#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

void hexdump_impl(void *data, size_t size, int type, FILE *file, const char *prefix);

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
  hexdump_impl(data, size, 0, file, "");
}

void hexdumpf_relative(void *data, size_t size, FILE *file)
{
  hexdump_impl(data, size, 1, file, "");
}

void hexdumpfp(void *data, size_t size, FILE *file, const char *prefix)
{
  hexdump_impl(data, size, 0, file, prefix);
}

void hexdump_impl(void *data, size_t size, int type, FILE *file, const char *prefix)
{
  for (size_t i = 0; i < size; i += 16)
  {
    if (type == 0)
      fprintf(file, "%s%p\t| ", prefix, (void *)((uint8_t *)data + i));
    else
      fprintf(file, "%s%08zx\t| ", prefix, i);
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
    fprintf(file, "|\t|");
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

void swap_bytes(void *data, size_t len)
{
  uint8_t *ptr = data, tmp;
  for (ssize_t i = 0; i < len / 2; i++)
  {
    tmp = ptr[i];
    ptr[i] = ptr[len - i - 1];
    ptr[len - i - 1] = tmp;
  }
}


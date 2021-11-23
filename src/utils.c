#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>


void hexdump(void *data, size_t size)
{
  for (size_t i = 0; i < size; i += 16)
  {
    printf("%p\t", (uint8_t *)data + i);
    int k;
    for (k = 0; k < 16 && (k + i) < size; k++)
    {
      printf("%02x", ((uint8_t *)data)[k + i]);
      if (k % 4 == 3) printf(" ");
    }
    for (; k < 16; k++)
    {
      printf("--");
      if (k % 4 == 3) printf(" ");
    }
    printf("\t|");
    for (k = 0; k < 16 && (k + i) < size; k++)
    {
      if (isprint(((char *)data)[k + i]))
        printf("%c", ((char *)data)[k + i]);
      else
        printf(".");
    }
    printf("|\n");
  }
}

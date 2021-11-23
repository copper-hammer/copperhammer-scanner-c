#include "utils.h"
#include "mcping.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  int16_t port = 25565;
  if (argc < 2) return EXIT_FAILURE;
  if (argc > 2) port = atoi(argv[2]);

  void *buffer = malloc(256);
  ssize_t l = mc_ping(argv[1], port == 0 ? 25565 : port, buffer, 256);
  printf("result: %ld\n", l);
  hexdump(buffer, 256);
  return EXIT_SUCCESS;
}

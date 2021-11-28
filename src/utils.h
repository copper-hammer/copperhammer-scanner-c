#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdlib.h>
#include <stdio.h>
#include "extras/cjson/cJSON.h"

void hexdump(void *data, size_t size);
void hexdump_relative(void *data, size_t size);
void hexdumpf(void *data, size_t size, FILE *file);
void hexdumpf_relative(void *data, size_t size, FILE *file);
void hexdumpfp(void *data, size_t size, FILE *file, const char *prefix);

void swap_bytes(void *data, size_t len);

#endif

#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdlib.h>
#include <stdio.h>

void hexdump(void *data, size_t size);
void hexdump_relative(void *data, size_t size);
void hexdumpf(void *data, size_t size, FILE *file);
void hexdumpf_relative(void *data, size_t size, FILE *file);
void hexdump_impl(void *data, size_t size, int type, FILE *file);

#endif

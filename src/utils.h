#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdlib.h>
#include <stdio.h>
#include "extras/cjson/cJSON.h"

void hexdump(const void *data, size_t size);
void hexdump_relative(const void *data, size_t size);
void hexdumpf(const void *data, size_t size, FILE *file);
void hexdumpf_relative(const void *data, size_t size, FILE *file);
void hexdumpfp(const void *data, size_t size, FILE *file, const char *prefix);

void swap_bytes(void *data, size_t len);
void mcs_chat_to_string(cJSON *message, char *result, size_t limit);

#endif

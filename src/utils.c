#include "utils.h"
#include "logging.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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

void ___mcs_chat_to_string(cJSON *message, char *result, size_t limit)
{
  if (cJSON_IsString(message))
  {
    DBG(LOG_TRACE, "message is string, concatenating directly");
    strncat(result, message->valuestring, limit);
  }
  else if (cJSON_IsArray(message))
  {
    DBG(LOG_TRACE, "message is an array, looping over");
    for (int i = 0; i < cJSON_GetArraySize(message); i++)
      ___mcs_chat_to_string(cJSON_GetArrayItem(message, i), result, limit);
  }
  else if (cJSON_IsObject(message))
  {
    DBG(LOG_TRACE, "message is an object, working with it");
    cJSON *text, *color, *bold, *italic,
          *underlined, *strikethrough, *obfuscated, *extra;
    if ((text = cJSON_GetObjectItem(message, "text")) == NULL)
      return;
    if (!cJSON_IsString(text)) return;
    if ((color = cJSON_GetObjectItem(message, "color")) != NULL
        && cJSON_IsString(color))
    {
      if (strcmp(color->valuestring, "black"))
        strncat(result, "\u00a70", limit);
      else if (strcmp(color->valuestring, "dark_blue"))
        strncat(result, "\u00a71", limit);
      else if (strcmp(color->valuestring, "dark_green"))
        strncat(result, "\u00a72", limit);
      else if (strcmp(color->valuestring, "dark_aqua"))
        strncat(result, "\u00a73", limit);
      else if (strcmp(color->valuestring, "dark_red"))
        strncat(result, "\u00a74", limit);
      else if (strcmp(color->valuestring, "dark_purple"))
        strncat(result, "\u00a75", limit);
      else if (strcmp(color->valuestring, "gold"))
        strncat(result, "\u00a76", limit);
      else if (strcmp(color->valuestring, "gray"))
        strncat(result, "\u00a77", limit);
      else if (strcmp(color->valuestring, "dark_gray"))
        strncat(result, "\u00a78", limit);
      else if (strcmp(color->valuestring, "blue"))
        strncat(result, "\u00a79", limit);
      else if (strcmp(color->valuestring, "green"))
        strncat(result, "\u00a7a", limit);
      else if (strcmp(color->valuestring, "aqua"))
        strncat(result, "\u00a7b", limit);
      else if (strcmp(color->valuestring, "red"))
        strncat(result, "\u00a7c", limit);
      else if (strcmp(color->valuestring, "light_purple"))
        strncat(result, "\u00a7d", limit);
      else if (strcmp(color->valuestring, "yellow"))
        strncat(result, "\u00a7e", limit);
      else if (strcmp(color->valuestring, "white"))
        strncat(result, "\u00a7f", limit);
    }
    
    if ((obfuscated = cJSON_GetObjectItem(message, "obfuscated")) != NULL
        && cJSON_IsTrue(obfuscated))
      strncat(result, "\u00a7k", limit);

    if ((bold = cJSON_GetObjectItem(message, "bold")) != NULL
        && cJSON_IsTrue(bold))
      strncat(result, "\u00a7l", limit);

    if ((strikethrough = cJSON_GetObjectItem(message, "strikethrough")) != NULL
        && cJSON_IsTrue(strikethrough))
      strncat(result, "\u00a7m", limit);

    if ((underlined = cJSON_GetObjectItem(message, "underline")) != NULL
        && cJSON_IsTrue(underlined))
      strncat(result, "\u00a7n", limit);

    if ((italic = cJSON_GetObjectItem(message, "italic")) != NULL
        && cJSON_IsTrue(italic))
      strncat(result, "\u00a7o", limit);
    
    strncat(result, text->valuestring, limit);
    
    if ((extra = cJSON_GetObjectItem(message, "extra")) != NULL
        && cJSON_IsArray(extra))
    {
      ___mcs_chat_to_string(extra, result, limit);
    }
  }
}

void mcs_chat_to_string(cJSON *message, char *result, size_t limit)
{
  memset(result, 0, limit);
  ___mcs_chat_to_string(message, result, limit);
}

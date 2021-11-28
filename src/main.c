#include "extras/cjson/cJSON.h"
#include "logging.h"
#include "utils.h"
#include "mcping.h"
#include "stb_ds.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/time.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT1 22
#define DEFAULT_PORT2 25565

typedef enum { 
  OMODE_JSON = 0,
  OMODE_TEXT = 1,
  OMODE_RAW = 2,
  OMODE_HEX = 3
} outmode_n;

typedef struct serverinfo_t {
  char host[64];
  uint16_t port;
  ssize_t response_len;
  uint8_t response[32768];
} serverinfo_t;

void usage(char *progname, FILE *file, int exitcode)
{
  fprintf(file, "Usage: %s ", progname);
  fprintf(file, "[-hvJTRX] [-o file] [-t thr] HOST [PORT_START] [PORT_END]\n\n");
  fprintf(file, "  -h\t\tShow this help\n");
  fprintf(file, "  -v\t\tBe verbose\n");
  fprintf(file, "  -J\t\tOutput in JSON\n");
  fprintf(file, "  -T\t\tOutput as text (default)\n");
  fprintf(file, "  -R\t\tOutput as raw data\n");
  fprintf(file, "  -X\t\tOutput as hexdump\n");
  fprintf(file, "  -o filename\tWrite results to this file\n");
  fprintf(file, "\t\t`-` means STDOUT (default)\n");
  fprintf(file, "  -t threads\tNOT IMPLEMENTED YET\n");
  fprintf(file, "  HOST\t\tHost to scan (def: "DEFAULT_HOST")\n");
  fprintf(file, "  PORT_START\tStart of port range (def: %d)\n", DEFAULT_PORT1);
  fprintf(file, "  PORT_END\tStart of port range (def: %d)\n", DEFAULT_PORT2);
  exit(exitcode);
}

void die(char *reason)
{
  fprintf(stderr, "Fatal: %s\n", reason);
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
  struct timeval tv_start, tv_stop;
  
  gettimeofday(&tv_start, NULL);

  int32_t range_start = DEFAULT_PORT1, range_end = DEFAULT_PORT2;
  char *host = DEFAULT_HOST;
  outmode_n output_mode = OMODE_TEXT;
  FILE *outfile = stdout;
  char *out_filename = NULL;
  int verbosity = 0;
  int n_threads = -1;
  
  int c;
  while ((c = getopt(argc, argv, "hvJTRXo:t:")) != -1)
  {
    switch (c)
    {
      case 'h': usage(argv[0], stdout, EXIT_SUCCESS); break;
      case 'v': verbosity++; break;
      case 'J': output_mode = OMODE_JSON; break;
      case 'T': output_mode = OMODE_TEXT; break;
      case 'R': output_mode = OMODE_RAW; break;
      case 'X': output_mode = OMODE_HEX; break;
      case 'o': out_filename = optarg; break;
      case 't': n_threads = atoi(optarg); break;
      case '?':
        if (optopt == 'o' || optopt == 't')
          fprintf(stderr, "Error: -%c requires an argument\n", optopt);
        else
          fprintf(stderr, "Error: Unknown parameter -%c\n", optopt);
        usage(argv[0], stderr, EXIT_FAILURE);
        break;
    }
  }
  loglevel = verbosity;

  for (int i = 0; i < argc; i++)
    DBG(LOG_TRACE, "argv[%d] = %s", i, argv[i]);
  
  
  if (optind + 1 <= argc) host = argv[optind];
  if (optind + 2 <= argc) range_start = atoi(argv[optind + 1]);
  if (optind + 3 <= argc) range_end = atoi(argv[optind + 2]);
  if (range_start <= 0 || range_end <= 0) die("Invalid port range");
  if (range_end < range_start)
  {
    int16_t tmp = range_end;
    range_end = range_start;
    range_start = tmp;
    DBG(LOG_WARN, "swapping ports range");
  }

  DBG(LOG_DEBUG, "Verbosity: %d (%s)", loglevel, get_loglevel_name(loglevel));
  DBG(LOG_DEBUG, "Threads: %d", n_threads);
  DBG(LOG_DEBUG, "Output mode: %d", output_mode);
  DBG(LOG_TRACE, "optind=%d", optind);
  DBG(LOG_TRACE, "argc=%d", argc);
  DBG(LOG_TRACE, "host=%s", host);
  DBG(LOG_TRACE, "ports=%d..%d", range_start, range_end);

  
  if (out_filename == NULL || strcmp(out_filename, "-") == 0)
  {
    outfile = stdout;
  }
  else
  {
    if ((outfile = fopen(out_filename, "w")) == NULL)
      die("Failed to open output file");
  }

  if (n_threads >= 0)
    die("Multi-thread support is not implemented yet");
  
  struct serverinfo_t *servers;
  uint8_t buffer[32768];
  ssize_t len;
  for (uint16_t port = range_start; port <= range_end; port++)
  {
    DBG(LOG_INFO, "Scanning %s:%d", host, port);
    struct socket_t *sock = socket_create(host, port, AF_INET);
    socket_settimeout(sock, 500);
    if (socket_connect(sock) < 0)
    {
      socket_close(sock);
      continue;
    }
    
    if (mcp_send_ping(sock, host, port, 756) < 0)
    {
      socket_close(sock);
      continue;
    }
    
    if (output_mode == OMODE_HEX)
      len = socket_recv(sock, buffer, 32768);
    else
      len = mcp_read_pong(sock, buffer, 32768);

    socket_close(sock);
    DBG(LOG_DEBUG, "Result: %ld", len);
    if (len >= 0)
    {
      struct serverinfo_t info;;
      strncpy(info.host, host, 64);
      info.host[63] = '\0';
      info.port = port;
      info.response_len = len;
      memcpy(info.response, buffer, 32768);
      arrput(servers, info);
    }
  }
  
  gettimeofday(&tv_stop, NULL);
  double execution_time = (double)(tv_stop.tv_usec - tv_start.tv_usec) / 1e6;
  execution_time += (double)(tv_stop.tv_sec - tv_start.tv_sec);

  struct serverinfo_t info;
  if (output_mode != OMODE_JSON)
  {
    cJSON *root;
    fprintf(outfile, "Execution time: %lf\n", execution_time);
    for (int i = 0; i < arrlen(servers); i++)
    {
      info = servers[i];
      switch (output_mode)
      {
        case OMODE_HEX:
          fprintf(outfile, "Server: %s:%d:\n", info.host, info.port);
          hexdumpf_relative(info.response, info.response_len, outfile);
          fprintf(outfile, "\n");
          break;

        case OMODE_RAW:
          fprintf(outfile, "Server: %s:%d (%zd bytes):\n", info.host, info.port, info.response_len);
          fwrite(info.response, 1, info.response_len, outfile);
          fprintf(outfile, "\n");
          break;

        case OMODE_TEXT:
          fprintf(outfile, "Server: %s:%d:\n", info.host, info.port);
          if ((root = cJSON_ParseWithLength((char *)info.response, info.response_len)) == NULL)
          {
            DBG(LOG_ERROR, "Invalid JSON received");
            fprintf(outfile, "Error: Invalid JSON\n");
            fprintf(outfile, "\n");
            continue;
          }
          // TODO: print all data as tree

          fprintf(outfile, "\n");
        
        // should not happen
        case OMODE_JSON:
          break;
      }
    }
  }
  else
  {
    cJSON *response = cJSON_CreateObject();
    if (response == NULL) die("Failed to create root JSON node");
    cJSON *j_results = cJSON_CreateArray();
    if (j_results == NULL) die("Failed to create results array");
    cJSON *j_duration = cJSON_CreateNumber(execution_time);
    if (j_duration == NULL) die("Failed to create execution time");
    cJSON_AddItemToObject(response, "duration", j_duration);
    
    cJSON *j_info, *jr_host, *jr_port;
    cJSON *jr_ver, *jr_vername, *jr_protocol, *jr_is_modded, *jr_mods;
    cJSON *jrp_online, *jrp_max, *jrp_sample, *jrps_uuid, *jrps_name;
    for (int i = 0; i < arrlen(servers); i++)
    {
      if ((j_info = cJSON_ParseWithLength((char *)info.response, info.response_len)) == NULL)
      {
        DBG(LOG_ERROR, "Failed to parse response %s:%d", info.host, info.port);
        continue;
      }
      jr_host = cJSON_CreateString(info.host);
      jr_port = cJSON_CreateNumber((double)info.port);
      jr_ver = cJSON_CreateObject();
      // TODO: deconstruct that lol
    }

  }

  if (outfile != stdout)
    fclose(outfile);
  
  arrfree(servers);
  return EXIT_SUCCESS;
}

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
#include <pthread.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT1 22
#define DEFAULT_PORT2 25565

typedef enum { 
  OMODE_JSON = 0,
  OMODE_TEXT = 1,
  OMODE_RAW = 2,
  OMODE_HEX = 3
} outmode_n;

typedef struct serverinfo_s {
  char host[64];
  uint16_t port;
  ssize_t response_len;
  uint8_t response[32768];
} serverinfo_t;

typedef struct scanner_thread_s {
  pthread_t *thread;
  pthread_mutex_t *lock;
  uint16_t port_start;
  uint16_t port_end;
  char host[64];
  outmode_n omode;
  serverinfo_t **servers;
} scanner_thread_t;

void *scanner_thread(void *params);

void usage(char *progname, FILE *f, int exitcode)
{
  fprintf(f, "Usage: %s ", progname);
  fprintf(f, "[-hvJTRX] [-o f] [-t thr] HOST [PORT_START] [PORT_END]\n\n");
  fprintf(f, "  -h\t\tShow this help\n");
  fprintf(f, "  -v\t\tBe verbose\n");
  fprintf(f, "  -J\t\tOutput in JSON\n");
  fprintf(f, "  -T\t\tOutput as text (default)\n");
  fprintf(f, "  -R\t\tOutput as raw data\n");
  fprintf(f, "  -X\t\tOutput as hexdump\n");
  fprintf(f, "  -o fname\tWrite results to this f\n");
  fprintf(f, "\t\t`-` means STDOUT (default)\n");
  fprintf(f, "  -t threads\tNOT IMPLEMENTED YET\n");
  fprintf(f, "  HOST\t\tHost to scan (def: "DEFAULT_HOST")\n");
  fprintf(f, "  PORT_START\tStart of port range (def: %d)\n", DEFAULT_PORT1);
  fprintf(f, "  PORT_END\tStart of port range (def: %d)\n", DEFAULT_PORT2);
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

  scanner_thread_t *scan_threads;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  serverinfo_t *servers = NULL;
  
  // XXX: Scan start here
  if (n_threads >= 0)
  {
    scan_threads = calloc(n_threads, sizeof(scanner_thread_t));
    int ports_per_thread = (range_end - range_start) / n_threads;
    int port = range_start;
    for (int i = 0; i < n_threads; i++)
    {
      scan_threads[i].lock = &lock;
      scan_threads[i].port_start = port;
      scan_threads[i].port_end = port + ports_per_thread;
      strncpy(scan_threads[i].host, host, 64);
      scan_threads[i].servers = &servers;
      DBG(LOG_WARN, "port range from %d to %d",
          port, port + ports_per_thread);
      port += ports_per_thread;
    }
    
    free(scan_threads);
  }
  else
  {
    scanner_thread_t thr; // It's single, but it is thread!
    thr.port_start = range_start;
    thr.port_end = range_end;
    strncpy(thr.host, host, 64);
    thr.servers = &servers;
    thr.omode = output_mode;
    scanner_thread(&thr);
  }
  // XXX: Scan end here
  
  gettimeofday(&tv_stop, NULL);
  double execution_time = (double)(tv_stop.tv_usec - tv_start.tv_usec) / 1e6;
  execution_time += (double)(tv_stop.tv_sec - tv_start.tv_sec);

  serverinfo_t info;
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
          fprintf(outfile, "Server: %s:%d (%zd bytes):\n",
              info.host, info.port, info.response_len);
          fwrite(info.response, 1, info.response_len, outfile);
          fprintf(outfile, "\n");
          break;

        case OMODE_TEXT:
          fprintf(outfile, "Server: %s:%d:\n", info.host, info.port);
          if ((root = cJSON_ParseWithLength(
                  (char *)info.response, info.response_len)) == NULL)
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
    cJSON *j_results = cJSON_AddArrayToObject(response, "results");
    if (j_results == NULL) die("Failed to create results array");
    cJSON *j_duration = cJSON_CreateNumber(execution_time);
    if (j_duration == NULL) die("Failed to create execution time");
    cJSON_AddItemToObject(response, "duration", j_duration);
    
    char modname[1024];
    char servername[8192];
    cJSON *j_info;
    for (int i = 0; i < arrlen(servers); i++)
    {
      info = servers[i];
      DBG(LOG_INFO, "Adding %s:%d", info.host, info.port);
      if ((j_info = cJSON_ParseWithLength(
              (char *)info.response, info.response_len)) == NULL)
      {
        DBG(LOG_ERROR, "Failed to parse response %s:%d", info.host, info.port);
        continue;
      }
      
      cJSON *ji_version = cJSON_GetObjectItem(j_info, "version");
      cJSON *ji_players = cJSON_GetObjectItem(j_info, "players");
      cJSON *ji_modinfo = cJSON_GetObjectItem(j_info, "modinfo");
      cJSON *ji_message = cJSON_GetObjectItem(j_info, "description");
      
      cJSON *server = cJSON_CreateObject();
      cJSON_AddStringToObject(server, "host", info.host);
      cJSON_AddNumberToObject(server, "port", info.port);
      mcs_chat_to_string(ji_message, servername, 8192);
      cJSON_AddStringToObject(server, "description", servername);
      cJSON *jic_message = cJSON_Duplicate(ji_message, true);
      cJSON_AddItemToObject(server, "description_raw", jic_message);
      

      cJSON *version = cJSON_AddObjectToObject(server, "version");
      cJSON_AddStringToObject(version, "name",
          cJSON_GetObjectItem(ji_version, "name")->valuestring);
      cJSON_AddNumberToObject(version, "protocol",
          cJSON_GetObjectItem(ji_version, "protocol")->valueint);
      cJSON_AddBoolToObject(version, "is_modded", ji_modinfo != NULL);
      if (ji_modinfo != NULL)
      {
        cJSON *modloader = cJSON_GetObjectItem(ji_modinfo, "type");
        cJSON *mods_list = cJSON_AddArrayToObject(version, "mods");
        char *modloader_name = modloader->valuestring;
        if (strcmp(modloader_name, "FML") == 0)
        {
          cJSON *modlist = cJSON_GetObjectItem(ji_modinfo, "modList");
          for (int i = 0; i < cJSON_GetArraySize(modlist); i++)
          {
            cJSON *mod = cJSON_GetArrayItem(modlist, i);
            snprintf(modname, 1024, "%s@%s",
                cJSON_GetObjectItem(mod, "modid")->valuestring,
                cJSON_GetObjectItem(mod, "version")->valuestring);
            cJSON_AddItemToArray(mods_list, cJSON_CreateString(modname));
          }
        }
      }

      cJSON *players = cJSON_AddObjectToObject(server, "players");
      cJSON_AddNumberToObject(players, "online",
          cJSON_GetObjectItem(ji_players, "online")->valuedouble);
      cJSON_AddNumberToObject(players, "max",
          cJSON_GetObjectItem(ji_players, "max")->valuedouble);
      cJSON *players_list = cJSON_AddArrayToObject(players, "sample");
      cJSON *players_sample = cJSON_GetObjectItem(ji_players, "sample");
      if (players_sample != NULL)
      {
        for (int i = 0; i < cJSON_GetArraySize(players_sample); i++)
        {
          cJSON *s_player = cJSON_GetArrayItem(players_sample, i);
          cJSON *player = cJSON_CreateObject();
          cJSON_AddStringToObject(player, "uuid",
              cJSON_GetObjectItem(s_player, "id")->valuestring);
          cJSON_AddStringToObject(player, "name",
              cJSON_GetObjectItem(s_player, "name")->valuestring);
          cJSON_AddItemToArray(players_list, player);
        }
      }

      cJSON_Delete(j_info);
      cJSON_AddItemToArray(j_results, server);
    }

    char *string = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);
    fwrite(string, 1, strlen(string), outfile);
    free(string);
  }

  if (outfile != stdout)
    fclose(outfile);
  
  arrfree(servers);
  return EXIT_SUCCESS;
}

void *scanner_thread(void *_params)
{
  scanner_thread_t *params = _params;
  uint8_t buffer[32768];
  ssize_t len;
  for (uint16_t port = params->port_start; port <= params->port_end; port++)
  {
    DBG(LOG_INFO, "Scanning %s:%d", params->host, port);
    socket_t *sock = socket_create(params->host, port, AF_INET);
    socket_settimeout(sock, 500);
    if (socket_connect(sock) < 0)
    {
      socket_close(sock);
      continue;
    }
    
    if (mcp_send_ping(sock, params->host, port, 756) < 0)
    {
      socket_close(sock);
      continue;
    }
    
    if (params->omode == OMODE_HEX)
      len = socket_recv(sock, buffer, 32768);
    else
      len = mcp_read_pong(sock, buffer, 32768);

    socket_close(sock);
    DBG(LOG_DEBUG, "Result: %ld", len);
    if (len >= 0)
    {
      serverinfo_t info;
      strncpy(info.host, params->host, 64);
      info.host[63] = '\0';
      info.port = port;
      info.response_len = len;
      memcpy(info.response, buffer, 32768);
      if (params->lock != NULL)
        pthread_mutex_lock(params->lock);
      arrput(*params->servers, info);
      if (params->lock != NULL)
        pthread_mutex_unlock(params->lock);
    }
  }
  return NULL;
}

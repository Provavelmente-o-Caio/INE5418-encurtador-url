#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int config_load(Config *cfg, const char *path) {
  // defaults
  strcpy(cfg->server_host, "127.0.0.1");
  cfg->server_port = 3000;
  cfg->proxy_port = 8080;
  cfg->max_clients = 10;

  FILE *f = fopen(path, "r");
  if (!f)
    return -1;

  char line[512];
  while (fgets(line, sizeof(line), f)) {
    char key[256], val[256];
    if (sscanf(line, "%255[^=]=%255s", key, val) != 2)
      continue;

    if (strcmp(key, "server_host") == 0)
      strncpy(cfg->server_host, val, 255);
    else if (strcmp(key, "server_port") == 0)
      cfg->server_port = atoi(val);
    else if (strcmp(key, "proxy_port") == 0)
      cfg->proxy_port = atoi(val);
    else if (strcmp(key, "max_clients") == 0)
      cfg->max_clients = atoi(val);
  }

  fclose(f);
  return 0;
}

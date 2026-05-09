#include "config.h"
#include "socket_server.h"
#include <stdio.h>
#include "cache.h"
int main(void) {
  Config cfg;

  if (config_load(&cfg, "config.txt") < 0) {
    printf("Erro carregando configurações");
    return -1;
  }

  cache_init(cfg.cache_size);
  printf("Cache size: %d\n", cfg.cache_size);
  socket_server_run(&cfg);
  return 0;
}

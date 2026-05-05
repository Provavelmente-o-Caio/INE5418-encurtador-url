#include "config.h"
#include "socket_server.h"
#include <stdio.h>

int main(void) {
  Config cfg;

  if (config_load(&cfg, "config.txt") < 0) {
    printf("Erro carregando configurações");
    return -1;
  }

  socket_server_run(&cfg);
  return 0;
}

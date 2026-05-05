#pragma once

typedef struct {
    char server_host[256];
    int server_port;
    int proxy_port;
    int max_clients;
} Config;

int config_load(Config *cfg, const char *path);

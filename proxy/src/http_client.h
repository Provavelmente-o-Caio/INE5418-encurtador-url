#pragma once
#include "config.h"

int http_post  (const Config *cfg, const char *path, const char *body, char *response_buf, int buflen);
int http_delete(const Config *cfg, const char *path, char *response_buf, int buflen);
int http_get   (const Config *cfg, const char *path, char *response_buf, int buflen);

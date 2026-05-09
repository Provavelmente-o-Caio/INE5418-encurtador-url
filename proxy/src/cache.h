#ifndef CACHE_H
#define CACHE_H

void cache_init(int size);

int cache_get(const char *code, char *url, int url_size);

void cache_put(const char *code, const char *url);

void cache_remove(const char *code);

#endif
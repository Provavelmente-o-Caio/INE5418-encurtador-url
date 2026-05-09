#include "cache.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char code[128];
    char url[2048];

    int valid;
    unsigned long long last_used;


} CacheEntry;

static unsigned long long global_counter = 0;

static CacheEntry *cache = NULL;

static int cache_size = 0;

static pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

void cache_init(int size) {
    cache_size = size;

    cache = calloc(size, sizeof(CacheEntry));
}

int cache_get(const char *code, char *url, int url_size) {
    pthread_mutex_lock(&cache_mutex);

    for (int i = 0; i < cache_size; i++) {

        if (cache[i].valid &&
            strcmp(cache[i].code, code) == 0) {

            strncpy(url, cache[i].url, url_size - 1);
            url[url_size - 1] = '\0';

            cache[i].last_used = ++global_counter;

            printf("[CACHE] HIT %s\n", code);

            pthread_mutex_unlock(&cache_mutex);

            return 1;
        }
    }

    printf("[CACHE] MISS %s\n", code);

    pthread_mutex_unlock(&cache_mutex);

    return 0;
}

void cache_put(const char *code, const char *url) {
    pthread_mutex_lock(&cache_mutex);

    // procura posição vazia
    for (int i = 0; i < cache_size; i++) {

        if (!cache[i].valid) {

            strncpy(cache[i].code, code,
                    sizeof(cache[i].code) - 1);
            cache[i].code[sizeof(cache[i].code) - 1] = '\0';

            strncpy(cache[i].url, url,
                    sizeof(cache[i].url) - 1);
            cache[i].url[sizeof(cache[i].url) - 1] = '\0';

            cache[i].valid = 1;

            cache[i].last_used = ++global_counter;

            pthread_mutex_unlock(&cache_mutex);

            return;
        }
    }

    // LRU: encontra o menos recentemente usado
    int lru = 0;

    for (int i = 1; i < cache_size; i++) {

        if (cache[i].last_used <
            cache[lru].last_used) {

            lru = i;
        }
    }

    printf("[CACHE] LRU substituiu %s\n",
           cache[lru].code);

    strncpy(cache[lru].code, code,
            sizeof(cache[lru].code) - 1);    
    cache[lru].code[sizeof(cache[lru].code) - 1] = '\0';

    strncpy(cache[lru].url, url,
            sizeof(cache[lru].url) - 1);    
    cache[lru].url[sizeof(cache[lru].url) - 1] = '\0';

    cache[lru].valid = 1;

    cache[lru].last_used = ++global_counter;


    pthread_mutex_unlock(&cache_mutex);
}

void cache_remove(const char *code) {
    pthread_mutex_lock(&cache_mutex);

    for (int i = 0; i < cache_size; i++) {

        if (cache[i].valid &&
            strcmp(cache[i].code, code) == 0) {

            cache[i].valid = 0;

            printf("[CACHE] INVALIDATE %s\n", code);

            break;
        }
    }

    pthread_mutex_unlock(&cache_mutex);
}
#include "circuit_breaker.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define MAX_FAILURES 3
#define OPEN_TIME_SECONDS 10

static int failures = 0;
static int open = 0;
static time_t opened_at = 0;

static pthread_mutex_t cb_mutex = PTHREAD_MUTEX_INITIALIZER;

int circuit_can_request(void) {
    pthread_mutex_lock(&cb_mutex);

    if (open) {
        time_t now = time(NULL);

        if (now - opened_at >= OPEN_TIME_SECONDS) {
            printf("[CIRCUIT BREAKER] Tempo expirado, tentando novamente\n");
            open = 0;
            failures = 0;
            pthread_mutex_unlock(&cb_mutex);
            return 1;
        }

        pthread_mutex_unlock(&cb_mutex);
        return 0;
    }

    pthread_mutex_unlock(&cb_mutex);
    return 1;
}

void circuit_success(void) {
    pthread_mutex_lock(&cb_mutex);

    failures = 0;
    open = 0;

    pthread_mutex_unlock(&cb_mutex);
}

void circuit_failure(void) {
    pthread_mutex_lock(&cb_mutex);

    failures++;

    printf("[CIRCUIT BREAKER] Falha %d/%d\n", failures, MAX_FAILURES);

    if (failures >= MAX_FAILURES) {
        open = 1;
        opened_at = time(NULL);
        printf("[CIRCUIT BREAKER] Circuito ABERTO\n");
    }

    pthread_mutex_unlock(&cb_mutex);
}
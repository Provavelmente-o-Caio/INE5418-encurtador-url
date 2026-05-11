#include "socket_server.h"
#include "parser_requisicoes.h"
#include "cache.h"
#include "circuit_breaker.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  Config *cfg;
  int fd;
} ThreadArgs;

static void *client_thread(void *arg) {
  ThreadArgs *arg_thread = (ThreadArgs *)arg;
  char buffer[4096] = {0};

  if (recv(arg_thread->fd, buffer, sizeof(buffer) - 1, 0) <= 0) {
    close(arg_thread->fd);
    free(arg_thread);
    printf("Falha na leitura do socket cliente\n");
    return NULL;
  }

  Requisition requisition;

  char response[4096] = {0};
  char http_body[4096] = {0};
  char http_response[4096] = {0};

  if (parse_requisition(buffer, &requisition)) {
    protocol_response(response, sizeof(response), 0, "comando invalido");
    send(arg_thread->fd, response, strlen(response), 0);
    close(arg_thread->fd);
    free(arg_thread);
    return NULL;
  }

  switch (requisition.type) {

  case POST: {
    snprintf(http_body, sizeof(http_body), "{\"url\":\"%s\"}",
             requisition.content);

    if (!circuit_can_request()) {
      protocol_response(response, sizeof(response), 0,
                        "servidor temporariamente indisponivel");
      break;
    }

    int status = http_post(arg_thread->cfg, "/urls", http_body,
                           http_response, sizeof(http_response));

    if (status < 0 || status >= 500)
      circuit_failure();
    else
      circuit_success();

    if (status == 201 || status == 200)
      protocol_response(response, sizeof(response), 1, http_response);
    else
      protocol_response(response, sizeof(response), 0, "erro ao encurtar");

    break;
  }

  case DELETE: {
    char path[128];

    snprintf(path, sizeof(path), "/urls/%s", requisition.content);

    if (!circuit_can_request()) {
      protocol_response(response, sizeof(response), 0,
                        "servidor temporariamente indisponivel");
      break;
    }

    int status = http_delete(arg_thread->cfg, path, http_response,
                             sizeof(http_response));

    if (status < 0 || status >= 500)
      circuit_failure();
    else
      circuit_success();

    if (status == 200) {
      cache_remove(requisition.content);
      protocol_response(response, sizeof(response), 1, "removido");
    } else {
      protocol_response(response, sizeof(response), 0, "erro ao remover");
    }

    break;
  }

  case GET: {
    if (requisition.content[0] == '\0' ||
        strcmp(requisition.content, "/urls") == 0 ||
        strcmp(requisition.content, "urls") == 0) {
      if (!circuit_can_request()) {
        protocol_response(response, sizeof(response), 0,
                          "servidor temporariamente indisponivel");
        break;
      }

      int status = http_get(arg_thread->cfg, "/urls", http_response,
                            sizeof(http_response));

      if (status < 0 || status >= 500)
        circuit_failure();
      else
        circuit_success();

      if (status == 200)
        protocol_response(response, sizeof(response), 1, http_response);
      else
        protocol_response(response, sizeof(response), 0, "erro ao listar");

      break;
    }

    char cached_url[2048];

    if (cache_get(requisition.content, cached_url, sizeof(cached_url))) {
      protocol_response(response, sizeof(response), 1, cached_url);
      break;
    }

    char path[128];

    snprintf(path, sizeof(path), "/urls/%s", requisition.content);

    if (!circuit_can_request()) {
      protocol_response(response, sizeof(response), 0,
                        "servidor temporariamente indisponivel");
      break;
    }

    int status = http_get(arg_thread->cfg, path, http_response,
                          sizeof(http_response));

    if (status < 0 || status >= 500)
      circuit_failure();
    else
      circuit_success();

    if (status == 200) {
      char url[2048] = {0};

      char *start = strstr(http_response, "\"url_original\":\"");

      if (start) {
        start += strlen("\"url_original\":\"");

        char *end = strchr(start, '"');

        if (end) {
          size_t len = end - start;

          strncpy(url, start, len);
          url[len] = '\0';

          cache_put(requisition.content, url);

          protocol_response(response, sizeof(response), 1, url);
        } else {
          protocol_response(response, sizeof(response), 0,
                            "codigo nao encontrado");
        }
      } else {
        protocol_response(response, sizeof(response), 0,
                          "codigo nao encontrado");
      }

    } else {
      protocol_response(response, sizeof(response), 0,
                        "codigo nao encontrado");
    }

    break;
  }

  }

  send(arg_thread->fd, response, strlen(response), 0);
  close(arg_thread->fd);
  free(arg_thread);
  return NULL;
}

int socket_server_run(Config *cfg) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    printf("Falha ao criar socket servidor\n");
    return -1;
  }

  int yes = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = INADDR_ANY,
      .sin_port = htons(cfg->proxy_port)
  };

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Falha ao conectar o socket do servidor ao destino\n");
    return -1;
  }

  listen(server_fd, cfg->max_clients);

  printf("Proxy escutando na porta %d\n", cfg->proxy_port);

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);

    if (client_fd < 0) {
      printf("Falha ao criar socket client\n");
      continue;
    }

    ThreadArgs *args = malloc(sizeof(ThreadArgs));
    args->cfg = cfg;
    args->fd = client_fd;

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, args);
    pthread_detach(tid);
  }

  close(server_fd);

  return 0;
}
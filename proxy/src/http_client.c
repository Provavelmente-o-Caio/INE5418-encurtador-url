#include "http_client.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int connect_to_server(const Config *cfg) {
  // Criando socket
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (sock_fd < 0) {
    printf("Falha ao criar socket\n");
    return -1;
  }

  // Conecta do servidor
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(cfg->server_port),
  };

  // converte endereço para padrão binário
  inet_pton(AF_INET, cfg->server_host, &addr.sin_addr);

  if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sock_fd);
    printf("Falha ao estabelecer conexão com o servidor\n");
    return -1;
  }
  return sock_fd;
}

static int send_request(const Config *cfg, const char *request,
                        char *response_buf, int buflen) {
  // Recebe o socket da conexão
  int sockfd = connect_to_server(cfg);
  if (sockfd < 0)
    return -1;

  // Envia request
  send(sockfd, request, strlen(request), 0);

  // recebe packets da respost
  int total = 0, n;
  while ((n = recv(sockfd, response_buf + total, buflen - total - 1, 0)) > 0) {
    total += n;
  }
  response_buf[total] = '\0';
  close(sockfd);

  // Extraindo resposta HTTP
  int status = 0;
  sscanf(response_buf, "HTTP/1.1 %d", &status);

  // extrai só o body (após o \r\n\r\n)
  char *body = strstr(response_buf, "\r\n\r\n");
  if (body) {
      body += 4; // pula o \r\n\r\n
      memmove(response_buf, body, strlen(body) + 1);
  }

  return status;
}

int http_post(const Config *cfg, const char *path, const char *body,
              char *response_buf, int buflen) {
  char request[4096];
  snprintf(request, sizeof(request),
           "POST %s HTTP/1.1\r\n"
           "Host: %s:%d\r\n"
           "Content-Type: application/json\r\n"
           "Content-Length: %zu\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           path, cfg->server_host, cfg->server_port, strlen(body), body);
  return send_request(cfg, request, response_buf, buflen);
}

int http_get(const Config *cfg, const char *path, char *response_buf,
             int buflen) {
  char request[1024];
  snprintf(request, sizeof(request),
           "GET %s HTTP/1.1\r\n"
           "Host: %s:%d\r\n"
           "Connection: close\r\n"
           "\r\n",
           path, cfg->server_host, cfg->server_port);
  return send_request(cfg, request, response_buf, buflen);
}

int http_delete(const Config *cfg, const char *path, char *response_buf,
                int buflen) {
  char request[1024];
  snprintf(request, sizeof(request),
           "DELETE %s HTTP/1.1\r\n"
           "Host: %s:%d\r\n"
           "Connection: close\r\n"
           "\r\n",
           path, cfg->server_host, cfg->server_port);
  return send_request(cfg, request, response_buf, buflen);
}

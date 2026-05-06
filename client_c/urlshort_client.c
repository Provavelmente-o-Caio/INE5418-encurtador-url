#include "urlshort_client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PROXY_IP    "127.0.0.1"
#define PROXY_PORT   8080
#define BUF_SIZE     1024

/* ── Abre conexão TCP com o proxy ─────────────────────── */
static int conecta(void) {
    int sockfd;
    struct sockaddr_in address;
    int result;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("oops: socket");
        return URLSHORT_ERR_SOCKET;
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = inet_addr(PROXY_IP);
    address.sin_port        = htons(PROXY_PORT);

    result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (result == -1) {
        perror("oops: connect");
        close(sockfd);
        return URLSHORT_ERR_CONNECT;
    }

    return sockfd;
}

/* ── encurta() ────────────────────────────────────────── */
int encurta(char *url_original, char *url_curta) {
    char msg[BUF_SIZE];
    char resp[BUF_SIZE];
    int sockfd;

    if (!url_original || !url_curta || url_original[0] == '\0')
        return URLSHORT_ERR_ARG;

    /* Mensagem: "POST <url>" */
    snprintf(msg, sizeof(msg), "POST %s", url_original);

    printf("Enviando: %s\n", msg);

    sockfd = conecta();
    if (sockfd < 0) return sockfd;

    write(sockfd, msg, BUF_SIZE);

    memset(resp, 0, sizeof(resp));
    read(sockfd, resp, sizeof(resp) - 1);

    printf("Resposta: %s\n", resp);

    close(sockfd);

    strncpy(url_curta, resp, CODE_MAX_LEN - 1);
    url_curta[CODE_MAX_LEN - 1] = '\0';

    return URLSHORT_OK;
}

/* ── resolve() ────────────────────────────────────────── */
int resolve(char *codigo_curto, char *url_original) {
    char msg[BUF_SIZE];
    char resp[BUF_SIZE];
    int sockfd;

    if (!codigo_curto || !url_original || codigo_curto[0] == '\0')
        return URLSHORT_ERR_ARG;

    /* Mensagem: "GET <codigo>" */
    snprintf(msg, sizeof(msg), "GET %s", codigo_curto);

    printf("Enviando: %s\n", msg);

    sockfd = conecta();
    if (sockfd < 0) return sockfd;

    write(sockfd, msg, BUF_SIZE);

    memset(resp, 0, sizeof(resp));
    read(sockfd, resp, sizeof(resp) - 1);

    printf("Resposta: %s\n", resp);

    close(sockfd);

    strncpy(url_original, resp, URL_MAX_LEN - 1);
    url_original[URL_MAX_LEN - 1] = '\0';

    return URLSHORT_OK;
}

/* ── remove_url() ─────────────────────────────────────── */
int remove_url(char *codigo_curto) {
    char msg[BUF_SIZE];
    char resp[BUF_SIZE];
    int sockfd;

    if (!codigo_curto || codigo_curto[0] == '\0')
        return URLSHORT_ERR_ARG;

    /* Mensagem: "DELETE <codigo>" */
    snprintf(msg, sizeof(msg), "DELETE %s", codigo_curto);

    printf("Enviando: %s\n", msg);

    sockfd = conecta();
    if (sockfd < 0) return sockfd;

    write(sockfd, msg, BUF_SIZE);

    memset(resp, 0, sizeof(resp));
    read(sockfd, resp, sizeof(resp) - 1);

    printf("Resposta: %s\n", resp);

    close(sockfd);

    return URLSHORT_OK;
}

/* ── Mensagens de erro ────────────────────────────────── */
const char *urlshort_strerror(int err) {
    switch (err) {
        case URLSHORT_OK:          return "Sucesso";
        case URLSHORT_ERR_SOCKET:  return "Falha ao criar socket";
        case URLSHORT_ERR_CONNECT: return "Falha ao conectar ao proxy";
        case URLSHORT_ERR_SEND:    return "Falha ao enviar dados";
        case URLSHORT_ERR_RECV:    return "Falha ao receber dados";
        case URLSHORT_ERR_PROTO:   return "Resposta inesperada do proxy";
        case URLSHORT_ERR_ARG:     return "Argumento invalido";
        case URLSHORT_ERR_SERVER:  return "Proxy retornou erro";
        default:                   return "Erro desconhecido";
    }
}
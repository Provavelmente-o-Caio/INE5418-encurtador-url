#ifndef URLSHORT_CLIENT_H
#define URLSHORT_CLIENT_H

/* =========================================================
 * urlshort_client.h  –  Biblioteca cliente para encurtamento
 *                       de URLs via proxy TCP (HTTP/1.1)
 *
 * Proxy alvo : 127.0.0.1:8080
 *
 * API pública:
 *   encurta()     → POST   /shorten
 *   resolve()     → GET    /resolve/<codigo>
 *   remove_url()  → DELETE /remove/<codigo>
 * ========================================================= */

#include <stddef.h>

/* ── Tamanhos máximos ─────────────────────────────────── */
#define URL_MAX_LEN   2048
#define CODE_MAX_LEN  64

/* ── Códigos de retorno ───────────────────────────────── */
#define URLSHORT_OK             0   /* Sucesso                          */
#define URLSHORT_ERR_SOCKET    -1   /* Falha ao criar socket            */
#define URLSHORT_ERR_CONNECT   -2   /* Falha ao conectar ao proxy       */
#define URLSHORT_ERR_SEND      -3   /* Falha ao enviar dados            */
#define URLSHORT_ERR_RECV      -4   /* Falha ao receber dados           */
#define URLSHORT_ERR_PROTO     -5   /* Resposta HTTP inesperada         */
#define URLSHORT_ERR_ARG       -6   /* Argumento inválido (NULL/vazio)  */
#define URLSHORT_ERR_SERVER    -7   /* Servidor retornou erro (4xx/5xx) */

/* ── Configuração da conexão ──────────────────────────── */
typedef struct {
    char   server_host[64];
    int    proxy_port;
    int    timeout_sec;     /* timeout de leitura em segundos (0 = sem timeout) */
} urlshort_cfg_t;

/* Inicializa cfg com os valores padrão (127.0.0.1:8080) */
void urlshort_cfg_init(urlshort_cfg_t *cfg);

/* Altera a configuração global usada pelas funções abaixo.
 * Passe NULL para restaurar o padrão.                     */
void urlshort_cfg_set(const urlshort_cfg_t *cfg);

/* ── Funções principais ───────────────────────────────── */

/*
 * encurta() – Envia url_original ao interceptador e grava
 *             o código curto em url_curta (buffer do chamador,
 *             tamanho mínimo CODE_MAX_LEN).
 * Retorna URLSHORT_OK ou código de erro negativo.
 */
int encurta(char *url_original, char *url_curta);

/*
 * resolve() – Envia codigo_curto ao interceptador e grava
 *             a URL original em url_original (buffer do chamador,
 *             tamanho mínimo URL_MAX_LEN).
 * Retorna URLSHORT_OK ou código de erro negativo.
 */
int resolve(char *codigo_curto, char *url_original);

/*
 * remove_url() – Remove o mapeamento de uma URL encurtada.
 * Retorna URLSHORT_OK ou código de erro negativo.
 */
int remove_url(char *codigo_curto);

/* Retorna string descritiva para um código de erro */
const char *urlshort_strerror(int err);

#endif /* URLSHORT_CLIENT_H */
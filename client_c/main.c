/* =========================================================
 * main.c  –  Programa de demonstração da biblioteca
 *            urlshort_client
 *
 * Uso:
 *   ./urlshort_demo encurta <url>
 *   ./urlshort_demo resolve  <codigo>
 *   ./urlshort_demo remove   <codigo>
 *
 * Opcional: define host e porta antes de chamar:
 *   ./urlshort_demo encurta <url> <host> <porta>
 * ========================================================= */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "urlshort_client.h"

#define BUF_SIZE 1024

/* Imprime código de erro de forma amigável */
static void print_err(const char *op, int code) {
    fprintf(stderr, "[ERRO] %s falhou (codigo %d): %s\n",
            op, code, urlshort_strerror(code));
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr,
            "Uso:\n"
            "  %s encurta <url_original> [host] [porta]\n"
            "  %s resolve  <codigo_curto> [host] [porta]\n"
            "  %s remove   <codigo_curto> [host] [porta]\n"
            "  %s lista                   [host] [porta]\n",
            argv[0], argv[0], argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    const char *op = argv[1];

    // "lista" não precisa de argumento — os demais sim
    if (strcmp(op, "lista") != 0 && argc < 3) {
        fprintf(stderr, "Erro: operação '%s' requer um argumento.\n", op);
        return EXIT_FAILURE;
    }

    /* ── Configuração (host e porta opcionais) ─────────── */
    urlshort_cfg_t cfg;
    urlshort_cfg_init(&cfg);

    // para "lista", host e porta estão em argv[2] e argv[3]
    // para os demais, estão em argv[3] e argv[4]
    int host_idx = (strcmp(op, "lista") == 0) ? 2 : 3;
    int port_idx = host_idx + 1;

    if (argc > host_idx) {
        strncpy(cfg.server_host, argv[host_idx], sizeof(cfg.server_host) - 1);
        cfg.server_host[sizeof(cfg.server_host) - 1] = '\0';
    }
    if (argc > port_idx) {
        cfg.proxy_port = atoi(argv[port_idx]);
    }

    urlshort_cfg_set(&cfg);

    /* ── Operação ──────────────────────────────────────── */
    const char *arg = argv[2];
    int ret;

    /* ── encurta ───────────────────────────────────────── */
    if (strcmp(op, "encurta") == 0) {

        char codigo[CODE_MAX_LEN] = {0};
        ret = encurta((char *)arg, codigo);

        if (ret == URLSHORT_OK) {
            printf("URL encurtada com sucesso!\n");
            printf("  Original : %s\n", arg);
            printf("  Codigo   : %s\n", codigo);
        } else {
            print_err("encurta", ret);
            return EXIT_FAILURE;
        }

    /* ── resolve ───────────────────────────────────────── */
    } else if (strcmp(op, "resolve") == 0) {

        char url[URL_MAX_LEN] = {0};
        ret = resolve((char *)arg, url);

        if (ret == URLSHORT_OK) {
            printf("Codigo resolvido com sucesso!\n");
            printf("  Codigo : %s\n", arg);
            printf("  URL    : %s\n", url);
        } else {
            print_err("resolve", ret);
            return EXIT_FAILURE;
        }

    /* ── remove ────────────────────────────────────────── */
    } else if (strcmp(op, "remove") == 0) {

        ret = remove_url((char *)arg);

        if (ret == URLSHORT_OK) {
            printf("Mapeamento removido com sucesso!\n");
            printf("  Codigo : %s\n", arg);
        } else {
            print_err("remove_url", ret);
            return EXIT_FAILURE;
        }

    /* ── lista ──────────────────────────────────────────── */
    } else if (strcmp(op, "lista") == 0) {

        char urls[BUF_SIZE];
        ret = lista(urls);

        if (ret == URLSHORT_OK) {
            printf("Lista de URLs:\n");
            printf("  %s\n", urls);
        } else {
            print_err("lista", ret);
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Operacao desconhecida: %s\n", op);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* =========================================================
 * main.c  –  Programa de demonstração da biblioteca
 *            urlshort_client
 *
 * Uso:
 *   ./urlshort_demo encurta <url>
 *   ./urlshort_demo resolve  <codigo>
 *   ./urlshort_demo remove   <codigo>
 * ========================================================= */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "urlshort_client.h"

/* Imprime código de erro de forma amigável */
static void print_err(const char *op, int code) {
    fprintf(stderr, "[ERRO] %s falhou (código %d): %s\n",
            op, code, urlshort_strerror(code));
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr,
            "Uso:\n"
            "  %s encurta <url_original>\n"
            "  %s resolve  <codigo_curto>\n"
            "  %s remove   <codigo_curto>\n",
            argv[0], argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    const char *op  = argv[1];
    const char *arg = argv[2];
    int ret;

    /* ── encurta ──────────────────────────────────────── */
    if (strcmp(op, "encurta") == 0) {

        char codigo[CODE_MAX_LEN] = {0};
        ret = encurta((char *)arg, codigo);

        if (ret == URLSHORT_OK) {
            printf("URL encurtada com sucesso!\n");
            printf("  Original : %s\n", arg);
            printf("  Código   : %s\n", codigo);
        } else {
            print_err("encurta", ret);
            return EXIT_FAILURE;
        }

    /* ── resolve ──────────────────────────────────────── */
    } else if (strcmp(op, "resolve") == 0) {

        char url[URL_MAX_LEN] = {0};
        ret = resolve((char *)arg, url);

        if (ret == URLSHORT_OK) {
            printf("Código resolvido com sucesso!\n");
            printf("  Código   : %s\n", arg);
            printf("  URL      : %s\n", url);
        } else {
            print_err("resolve", ret);
            return EXIT_FAILURE;
        }

    /* ── remove ───────────────────────────────────────── */
    } else if (strcmp(op, "remove") == 0) {

        ret = remove_url((char *)arg);

        if (ret == URLSHORT_OK) {
            printf("Mapeamento removido com sucesso!\n");
            printf("  Código   : %s\n", arg);
        } else {
            print_err("remove_url", ret);
            return EXIT_FAILURE;
        }

    } else {
        fprintf(stderr, "Operação desconhecida: %s\n", op);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
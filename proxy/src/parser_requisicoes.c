#include "parser_requisicoes.h"
#include <stdio.h>
#include <string.h>

int parse_requisition(const char *requisition, Requisition *out) {
  char requisition_type[32];
  if (sscanf(requisition, "%31s %2047[^\n]", requisition_type, out->content) <
      1) {
    return -1;
  }

  if (strcmp(requisition_type, "POST") == 0) {
    out->type = POST;
  } else if (strcmp(requisition_type, "DELETE") == 0) {
    out->type = DELETE;
  } else if (strcmp(requisition_type, "GET") == 0) {
    out->type = GET;
  } else {
    return -1;
  }

  return 0;
}

int protocol_response(char *buf, int buflen, int ok, const char *payload) {
  return snprintf(buf, buflen, "%s %s\n", ok ? "OK" : "ERR", payload);
}

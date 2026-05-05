#pragma once

#include "http_client.h"

typedef enum { POST, DELETE, GET } RequisitionType;

typedef struct {
  RequisitionType type;
  char content[2048];
} Requisition;

int parse_requisition(const char *requisition, Requisition *out);

int protocol_response(char *buf, int buflen, int ok, const char *payload);

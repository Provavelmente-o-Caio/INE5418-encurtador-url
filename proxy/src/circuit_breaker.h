#ifndef CIRCUIT_BREAKER_H
#define CIRCUIT_BREAKER_H

int circuit_can_request(void);
void circuit_success(void);
void circuit_failure(void);

#endif
#ifndef SERVER_H_
#define SERVER_H_
#include "ib.h"
#include <netinet/in.h>

int run_server(struct Resource *res, struct addrinfo *rp);
#endif  /*server.h*/
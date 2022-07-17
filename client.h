#ifndef CLIENT_H_
#define CLIENT_H_
#include "ib.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
int run_client(struct Resource *res, struct addrinfo *rp);
#endif /* client.h */
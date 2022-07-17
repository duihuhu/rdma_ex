#ifndef SERVER_H_
#define SERVER_H_
#include "ib.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

int run_server(struct Resource *res);
#endif  /*server.h*/
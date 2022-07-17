/*************************************************************************
	> File Name: sock.h
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Wed 06 Jul 2022 01:42:39 PM UTC
 ************************************************************************/
#ifndef SOCK_H_
#define SCOK_H_
#include <stdint.h>
#include "ib.h"
struct addrinfo* socket_connect(char *server_name, uint32_t tcp_port);
int sock_read(int sockfd, void *buffer, int len);
int sock_write(int sockfd, void *buffer,int len);
struct addrinfo* init_socket();
uint64_t htonll(uint64_t n);
uint64_t ntohll(uint64_t n);
#endif

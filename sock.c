/*************************************************************************
	> File Name: sock.c
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Wed 06 Jul 2022 01:43:47 PM UTC
 ************************************************************************/
#include "sock.h"
#include "ib.h"
#include "config.h"
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
// extern struct Resource res;

uint64_t htonll(uint64_t n)
{
	return (((uint64_t)htonl(n))<<32) | htonl(n>>32);
}
uint64_t ntohll(uint64_t n)
{
	return (((uint64_t)ntohl(n))<<32) | ntohl(n>>32);
}

struct addrinfo* socket_connect(char *server_name, uint32_t tcp_port)
{
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *addr_res=NULL, *rp=NULL;
	int ret;
	int sockfd = -1;
	char port[10];
	// int listenfd = -1;
	if (sprintf(port, "%d", tcp_port)<0) {
		fprintf(stdout, "port cast failed\n");
		return rp;
	}
	ret = getaddrinfo(server_name, port, &hints, &addr_res);
	if (ret) {
		fprintf(stdout, "getaddrinfo failed\n");
		return rp;
	}
	for (rp=addr_res; rp!=NULL; rp=rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd >= 0) {
			// if (!server_name) {
			// 	if (bind(sockfd, rp->ai_addr, rp->ai_addrlen)<0) {
			// 		fprintf(stdout, "server bind failed\n");
			// 		return -1;
			// 	}
			// 	if (listen(sockfd,5) ==-1) {
			// 		fprintf (stdout, "listen failed\n");
			// 		return -1;
			// 	}
			// 	listenfd = sockfd;
			// } else {
			// 	if (connect(sockfd, rp->ai_addr, rp->ai_addrlen)<0) {
			// 		fprintf(stdout, "client connect failed\n");
			// 		close(sockfd);
			// 		return  - 1; 
			// 	}
			// 	listenfd = sockfd;
			// }
			break;
		}
	}
	free(addr_res);
	return rp;

}

int sock_read(int sockfd, void *buffer, int len)
{
	int l_bytes = len;
	int r_bytes;
	char *buf = buffer; 
	while (l_bytes > 0) {
		r_bytes = read(sockfd, buf, l_bytes);
		if (r_bytes < 0) {
			if (r_bytes==-1 && errno==EINTR)
				continue;
			else
				return -1;
		} else if (r_bytes == 0)
			break;
		l_bytes -= r_bytes;
		buf += r_bytes;
	}
	return 0;
}

int sock_write(int sockfd, void *buffer, int len)
{
	int w_bytes;
	int l_bytes = len;
	const char *buf = buffer;
	while (l_bytes > 0) {
		w_bytes = write(sockfd, buf, l_bytes);
		if (w_bytes <= 0) {
			if(w_bytes==-1 && errno==EINTR)
				continue;
			else
				return -1;
		}
		l_bytes -= w_bytes;
		buf += w_bytes;
	}
	return 0;
}

struct addrinfo* init_socket()
{
	struct addrinfo *rp = NULL;
	if (!cfg.server_name) {
		rp = socket_connect(NULL, cfg.tcp_port);
		if (!rp) {
			fprintf(stdout, "failed to establish server\n");
			return rp;
		}
	} else {
		rp = socket_connect(cfg.server_name, cfg.tcp_port);
		if (!rp) {
			fprintf(stdout, "failed to establish connect\n");
			return rp;
		}
	}
	return rp;
}
/*************************************************************************
	> File Name: sock.c
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Wed 06 Jul 2022 01:43:47 PM UTC
 ************************************************************************/
#include "sock.h"
#include "ib.h"
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
int socket_connect(struct Resource *res, char *server_name, uint32_t tcp_port)
{
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *addr_res=NULL, *rp;
	int ret;
	int sockfd = -1;
	char port[10];
	if (sprintf(port, "%d", tcp_port)<0) {
		fprintf(stdout, "port cast failed\n");
		return -1;
	}
	ret = getaddrinfo(server_name, port, &hints, &addr_res);
	if (ret) {
		fprintf(stdout, "getaddrinfo failed\n");
		return -1;
	}
	for (rp=addr_res; rp!=NULL; rp=rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd >= 0) {
			if (!server_name) {
				struct sockaddr_in c_addr;
				socklen_t c_addr_len = sizeof(struct sockaddr_in);
				if (bind(sockfd, rp->ai_addr, rp->ai_addrlen)<0) {
					fprintf(stdout, "server bind failed\n");
					return -1;
				}
				if (listen(sockfd,5) ==-1) {
					fprintf (stdout, "listen failed\n");
					return -1;
				}
				res->sockfd = accept(sockfd, (struct sockaddr*)&c_addr, &c_addr_len);
				if (res->sockfd < 0) {
					fprintf( stdout, "accept failed\n");
					 return -1;
				}
			} else { 
				if (connect(sockfd, rp->ai_addr, rp->ai_addrlen)<0) {
					fprintf(stdout, "client connect failed\n");
					close(sockfd);
					return  - 1; 
				}
				res->sockfd = sockfd;
			}
		}
	}
	free(addr_res);
	return sockfd;

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

#include "server.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "config.h"
#include "ib.h"
void *server_func(void *mul_args){
    int ret;
    struct MulArgs *args;
    args = (struct MulArgs *) mul_args;
    ret = init_ib(args->res, args->thread_id);
    if (ret < 0) {
        fprintf(stderr, "server thread %d faild init ib\n", args->thread_id);
        return (void*)-1;
    }
    return 0;
}

int run_server (struct Resource *res, struct addrinfo *ad)
{
    int i = 0;
    struct MulArgs *mul_args;
    mul_args = malloc(cfg.num_threads * sizeof(struct MulArgs));
    pthread_t   *threads = NULL;
    threads = (pthread_t *) calloc (cfg.num_threads, sizeof(pthread_t));
    if (threads == NULL)
        fprintf(stderr,  "Failed to allocate threads.");
    
    struct sockaddr_in c_addr;
    int listenfd;
    socklen_t c_addr_len = sizeof(struct sockaddr_in);
    int sockfd = -1;

    struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *addr_res=NULL, *rp=NULL;
	// struct addrinfo *addr_res=NULL, *ap;

	int ret;
	char port[10];
	// int listenfd = -1;
	if (sprintf(port, "%d", cfg.tcp_port)<0) {
		fprintf(stdout, "port cast failed\n");
		return -1;
	}
	ret = getaddrinfo(cfg.server_name, port, &hints, &addr_res);
	if (ret) {
		fprintf(stdout, "getaddrinfo failed\n");
		return -1;
	}

    for (rp=addr_res; rp!=NULL; rp=rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd >= 0) {
            if (bind(sockfd, rp->ai_addr, rp->ai_addrlen)<0) {
                fprintf(stdout, "server bind failed\n");
                return -1;
            }
        } else
            return -1;
    }

    while (1) {
    	listenfd = accept(sockfd, (struct sockaddr*)&c_addr, &c_addr_len);
		if (listenfd < 0) {
			fprintf( stdout, "accept failed\n");
			return -1;
		}
        mul_args[i].sockfd = listenfd;
        mul_args[i].res = &res[i];
        mul_args[i].thread_id = i;
        if((pthread_create(&threads[i], NULL, server_func, (void *)&mul_args[i])) == -1){
			printf("create error!\n");
		}
		else{
			printf("create success!\n");
			i++;
		}
    }

    free (threads);

    return 0;

}
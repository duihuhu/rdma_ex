#include "ib.h"
#include "client.h"
#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *client_func(void *mul_args) {
    int ret;
    struct MulArgs *args;
    args = (struct MulArgs *) mul_args;
    ret = init_ib(args->res);
    if (ret < 0) {
        fprintf(stderr, "client thread %d faild init ib\n", args->thread_id);
        return (void*)-1;
    }
    ret = com_op(args->res);
    if (ret < 0) {
        fprintf(stderr, "communicate operation failed\n");
        return (void*)-1;
    }
    return 0;
}

int run_client(struct Resource *res, struct addrinfo *ad)
{
    int ret = 0;
    long i = 0;
    
    pthread_t *threads = NULL;
    pthread_attr_t  attr;
    void    *status;

    struct MulArgs *mul_args;
    mul_args = malloc(cfg.num_threads * sizeof(struct MulArgs));

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    threads = (pthread_t *) calloc(cfg.num_threads, sizeof(pthread_t));
    if (threads == NULL)
        fprintf(stderr,  "Failed to allocate threads.");


    struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};
	struct addrinfo *addr_res=NULL;
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
    int *sockfd;
    sockfd = (int *)malloc(cfg.num_threads * sizeof(int));
    for (i = 0; i < cfg.num_threads; i++) {
        struct addrinfo *rp;
        for (rp=addr_res; rp!=NULL; rp=rp->ai_next) {
		    sockfd[i] = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sockfd[i] >= 0) {
                if (connect(sockfd[i], rp->ai_addr, rp->ai_addrlen)<0) {
                    fprintf(stdout, "client connect failed\n");
                    close(sockfd[i]);
                    return  - 1; 
                }
            } else
                return -1;
        }
        res[i].sockfd = sockfd[i];
        mul_args[i].res = &res[i];
        mul_args[i].sockfd = sockfd[i];
        mul_args[i].thread_id = i;
        ret = pthread_create(&threads[i], &attr, client_func, (void *)&mul_args[i]);
        if (ret != 0) 
            fprintf(stderr, "Failed to create client_thread[%ld]", i);
    }

    bool thread_ret_normally = true;
    for (i = 0; i < cfg.num_threads; i++) {
        ret = pthread_join (threads[i], &status);
        if (ret != 0) 
            fprintf(stderr, "Failed to join thread[%ld].", i);
        if ((long)status != 0) {
            thread_ret_normally = false;
            fprintf(stdout, "client_thread[%ld]: failed to execute", i);
        }
    }

    if (thread_ret_normally == false) {
        goto error;
    }

    pthread_attr_destroy(&attr);
    free (threads);

    return 0;

 error:
    if (threads != NULL) {
        free(threads);
    }
    pthread_attr_destroy(&attr);
    
    return -1;
}
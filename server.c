#include "server.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
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

int run_server (struct Resource *res, int sockfd)
{
    int i = 0;
    struct MulArgs *mul_args;
    mul_args = malloc(cfg.num_threads * sizeof(struct MulArgs));
    pthread_t   *threads = NULL;
    threads = (pthread_t *) calloc (cfg.num_threads, sizeof(pthread_t));
    if (threads == NULL)
        fprintf(stderr,  "Failed to allocate threads.");
        goto error;
    while (1) {
        int listenfd;
        struct sockaddr_in c_addr;
        socklen_t c_addr_len = sizeof(struct sockaddr_in);
    	listenfd = accept(sockfd, (struct sockaddr*)&c_addr, &c_addr_len);
		if (listenfd < 0) {
			fprintf( stdout, "accept failed\n");
			return -1;
		}
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

 error:
    if (threads != NULL) {
        free(threads);
    }    
    return -1;
}
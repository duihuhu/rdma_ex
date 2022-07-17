#include "ib.h"
#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *client_func(void *mul_args) {
    int ret;
    struct MulArgs *args;
    args = (struct MulArgs *) mul_args;
    ret = init_ib(args->res);
    if (ret < 0) {
        fprintf(stderr, "client thread %d faild init ib\n", args->thread_id);
        return -1;
    }
    return 0;
}

int run_client(struct Resource *res, int sockfd)
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

    for (i = 0; i < cfg.num_threads; i++) {
        mul_args[i].res = &res[i];
        mul_args[i].sockfd = sockfd;
        mul_args[i].thread_id = i;
        ret = pthread_create (&threads[i], &attr, client_func, (void *)&mul_args[i]);
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
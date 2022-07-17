#include "server.h"
#include <pthread.h>
#include <stdlib.h>
#include "config.h"
#include "ib.h"
int server_func(void *mul_args){
    return 0;
}

int run_server (struct Resource *res, int sockfd)
{
    int   ret         = 0;
    long  i           = 0;

    pthread_t           *threads = NULL;
    pthread_attr_t       attr;
    void                *status;
    struct MulArgs mul_args;
    memset(&mul_args, 0, sizeof(struct MulArgs));
    mul_args.res = res;
    mul_args.sockfd = sockfd;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

    threads = (pthread_t *) calloc (cfg.num_threads, sizeof(pthread_t));
    check (threads != NULL, "Failed to allocate threads.");

    for (i = 0; i < num_threads; i++) {
        mul_args.thread_id = i;
        ret = pthread_create (&threads[i], &attr, server_func, (void *)&mul_args);
        check (ret == 0, "Failed to create server_thread[%ld]", i);
    }

    bool thread_ret_normally = true;
    for (i = 0; i < num_threads; i++) {
        ret = pthread_join (threads[i], &status);
        check (ret == 0, "Failed to join thread[%ld].", i);
        if ((long)status != 0) {
            thread_ret_normally = false;
            fprintf(stdout, "server_thread[%ld]: failed to execute", i);
        }
    }

    if (thread_ret_normally == false) {
        goto error;
    }

    pthread_attr_destroy    (&attr);
    free (threads);

    return 0;

 error:
    if (threads != NULL) {
        free(threads);
    }
    pthread_attr_destroy    (&attr);
    
    return -1;
}
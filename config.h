/*************************************************************************
	> File Name: config.h
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Mon 04 Jul 2022 01:34:28 PM UTC
 ************************************************************************/
#ifndef CONFIG_H_
#define CONFIG_H_
#include <stdbool.h>
#include <stdint.h>
#include "ib.h"
struct Config {
	char	*server_name;
	char	*dev_name;
	uint32_t	tcp_port;
	int		msg_size;
	int		num_threads;
	char	*op_type;
  int gid_idx;
  int msg_count;
};
extern struct Config cfg;

struct MulArgs {
	struct Resource *res;
	int sockfd;
	int thread_id;
};


#endif /*config.h*/

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
struct Config {
	char	*server_name;
	char	*dev_name;
	uint32_t	tcp_port;
	int		msg_size;
	int		num_threads;
	char	*op_type;
};
extern struct Config cfg;




#endif /*config.h*/

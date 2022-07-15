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
	bool	is_server;
	uint32_t	tcp_port;
	char	*dev_name;
	int		msg_size;
	int		threads;
	char	*server_name;
};
extern struct Config cfg;




#endif /*config.h*/

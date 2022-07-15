/*************************************************************************
	> File Name: main.c
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Mon 04 Jul 2022 01:00:42 PM UTC
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "config.h"
#include "ib.h"

struct Config cfg = {
	NULL, /* dev_name */
	NULL, /* server_name */
	7000, /* tcp_port */
	1, 	/*size */
	1,  /*threads */
};

void init_config();
int main(int argc, char *argv[]){
	int ret = 0;
	init_config(argc, argv);
	ret = init_ib();
	if (ret == -1) {
		fprintf(stdout,"init ib devices failed");
	}
	return 0;
}

static void usage(const char *argv0)
{
	fprintf(stdout, "Usage:\n");
	fprintf(stdout, " %s start a server and wait for connection\n", argv0);
	fprintf(stdout, " %s <host> connect to server at <host>\n", argv0);
	fprintf(stdout, "\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, " -p, --port <port> listen on/connect to port <port> default 7000\n");
	fprintf(stdout, " -d, --ib-dev <dev> use IB device <dev> (default first device found)\n");
	fprintf(stdout, " -s, --msg-size  (default 1 alloc numbers of page)\n");
	fprintf(stdout, " -t, --threads (defulat 1)\n");
}

void init_config(int argc, char *argv[])
{
	while (1)
	{
		int c;
		static struct option long_options[] = {
			{.name = "port", .has_arg = 1, .val = 'p'},
			{.name = "ib-dev", .has_arg = 1, .val = 'd'},
			{.name = "msg-size", .has_arg = 1, .val = 's'},
			{.name = "threads", .has_arg = 1, .val = 'c'},
			{.name = NULL, .has_arg = 0, .val = '\0'}
        };
		c = getopt_long(argc, argv, "p:d:s:t:", long_options, NULL);
		if (c == -1)
			break;
		switch (c)
		{
		case 'p':
			cfg.tcp_port = strtoul(optarg, NULL, 0);
			break;
		case 'd':
			cfg.dev_name = strdup(optarg);
			break;
		case 's':
			cfg.msg_size = strtoul(optarg, NULL, 0);
			break;
		case 't':
			cfg.threads = strtoul(optarg, NULL, 0);
			break;
		default:
			usage(argv[0]);
			return;
		}
	}
	/* parse the last parameter (if exists) as the server name */
	if (optind == argc - 1)
		cfg.server_name = argv[optind];
    if(cfg.server_name){
        printf("servername=%s\n",cfg.server_name);
    }
	else if (optind < argc)
	{
		usage(argv[0]);
		return;
	}
	fprintf(stdout, "output config %u,%s,%d,%d,%s\n",cfg.tcp_port,cfg.dev_name, cfg.msg_size, cfg.threads, cfg.server_name);
}

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
	"SR", /* op type */
};

int init_config();
int main(int argc, char *argv[]){
	int ret = 0;
	ret = init_config(argc, argv);
	if (ret == -1){
		fprintf(stdout, "re init config \n");
		return 0;
	}

	ret = init_ib();

	if (ret == -1) {
		fprintf(stdout,"init ib devices failed\n");
	}

	if (!strcmp(cfg.op_type, IB_OP_SR)) {
		if (!cfg.server_name) {
			return 0;
		};

	} else if (!strcmp(cfg.op_type, IB_OP_RD)) {
		if (cfg.server_name) {
			ck_cs_wire();
			/* read contens of server's buffer */
			if (post_send(IBV_WR_RDMA_READ))
			{
				fprintf(stderr, "failed to post SR read\n");
				return -1;
			}
			if (poll_completion())
			{
				fprintf(stderr, "poll completion failed read\n");
				return -1;
			}
			fprintf(stdout, "Contents of server's buffer: '%s'\n", res.ib_buf);
		} else {

			memset(res.ib_buf, 'T', res.ib_buf_size);
			fprintf(stdout, "res buf %s\n", res.ib_buf);
			ck_cs_wire();
		}

	} else if (!strcmp(cfg.op_type, IB_OP_WR)) {
		if (cfg.server_name) {
			memset(res.ib_buf, 'W', res.ib_buf_size);
			fprintf(stdout, "res buf %s\n", res.ib_buf);
			if (post_send(IBV_WR_RDMA_WRITE))
			{
				fprintf(stderr, "failed to post SR 3\n");
				return -1;
			}
			if (poll_completion())
			{
				fprintf(stderr, "poll completion failed 3\n");
				return -1;
			}
		}
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
	fprintf(stdout, " -t, --num_threads (defulat 1)\n");
	fprintf(stdout, " -o, --op-type (default type send/recv)\n");
}

int init_config(int argc, char *argv[])
{
	while (1)
	{
		int c;
		static struct option long_options[] = {
			{.name = "port", .has_arg = 1, .val = 'p'},
			{.name = "ib-dev", .has_arg = 1, .val = 'd'},
			{.name = "msg-size", .has_arg = 1, .val = 's'},
			{.name = "num_threads", .has_arg = 1, .val = 'c'},
			{.name = "op-type", .has_arg = 1, .val = 'o'},
			{.name = NULL, .has_arg = 0, .val = '\0'}
        };
		c = getopt_long(argc, argv, "p:d:s:t:o:", long_options, NULL);
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
			cfg.num_threads = strtoul(optarg, NULL, 0);
			break;
		case 'o':
			cfg.op_type = strdup(optarg);
			break;
		default:
			usage(argv[0]);
			return -1;
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
		return -1;
	}
	fprintf(stdout, "output config %u,%s,%d,%d,%s,%s\n",cfg.tcp_port,cfg.dev_name, cfg.msg_size, cfg.num_threads, cfg.server_name, cfg.op_type);
	return 0;
}

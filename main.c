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
#include "sock.h"
#include "config.h"
#include "ib.h"
#include "server.h"
#include "client.h"
#include <netdb.h>

struct Config cfg = {
	NULL, /* dev_name */
	NULL, /* server_name */
	7000, /* tcp_port */
	1, 	/*size */
	1,  /*threads */
	"SR", /* op type */
  '1',
};
int init_config();
void statics(struct Resource *res);
int main(int argc, char *argv[]){
	struct Resource *res;
	int ret = 0;
	// struct addrinfo *addr_res=NULL;
	// memset(&rp, 0, sizeof(struct addrinfo *));
	ret = init_config(argc, argv);
	if (ret == -1){
		fprintf(stdout, "re init config \n");
		return 0;
	}
    res = malloc(cfg.num_threads * sizeof(struct Resource));
    // ret = init_socket(addr_res);
	if (ret < 0) {
		fprintf(stdout, "init socket failed\n");
	}
  if (cfg.server_name) {
    run_client(res);
    statics(res);
  } else {
    run_server(res);
    statics(res);
  }
	// ret = init_ib(res, sockfd);
	return 0;
}

void statics(struct Resource *res)
{
	int i;
	double latency = 0.0;
	double throughtput = 0.0;
	for (i=0; i < cfg.num_threads; ++i) {
		latency = latency + res[i].duration;
		throughtput = throughtput + res[i].tp;
	}
	fprintf(stdout, "latency %lf %lf\n", latency/cfg.num_threads, throughtput/cfg.num_threads);
	return;
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
	fprintf(stdout, " -s, --msg-size  (default 1 alloc msg size)\n");
	fprintf(stdout, " -t, --num_threads (defulat 1)\n");
	fprintf(stdout, " -o, --op-type (default type send/recv)\n");
  fprintf(stdout, " -g, --gid-idx\n");
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
			{.name = "num_threads", .has_arg = 1, .val = 't'},
			{.name = "op-type", .has_arg = 1, .val = 'o'},
      {.name = "gid-idx", .has_arg = 1, .val = 'g'},
			{.name = NULL, .has_arg = 0, .val = '\0'}
        };
		c = getopt_long(argc, argv, "p:d:s:t:o:g:", long_options, NULL);
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
    case 'g':
      cfg.gid_id = strtoul(optarg, NULL, 0);
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

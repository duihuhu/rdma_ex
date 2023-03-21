/*************************************************************************
	> File Name: ib.h
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Mon 04 Jul 2022 01:59:09 PM UTC
 ************************************************************************/
#ifndef IB_H_
#define IB_H_
#include <infiniband/verbs.h>
#define PAGE_SIZE	4096
#define	IB_PORT		1
#define IB_SL		0
#define IB_MTU		IBV_MTU_4096
#define MAX_POLL_CQ_TIMEOUT 2000000000
#define SYNC_MES	"sync"
#define IB_OP_SR	"SR"
#define IB_OP_RD	"READ"
#define IB_OP_WR	"WRITE"
#define IB_OP_WI	"IWRITE"
#define IB_OP_CAS	"CAS"
#define rx_depth 500
struct QpInfo {
	uint16_t lid;
	uint32_t qp_num;
	uint32_t rkey;
	uint64_t raddr;
  uint8_t gid[16];
};

struct Resource {
	struct	ibv_context	*ctx;
	struct	ibv_pd	*pd;
	struct	ibv_mr	*mr;
	struct	ibv_cq	*cq;
	struct	ibv_qp	*qp;
	struct	ibv_device_attr	dev_attr;
	struct	ibv_port_attr	port_attr;
	char	*ib_buf;
	uint64_t buf;
	int		ib_buf_size;
	int		sockfd;
	uint32_t	rkey;
	uint64_t	raddr;
	struct QpInfo qpinfo;
	double duration;
	double tp;
	
};
// extern struct Resource res;

int init_ib(struct Resource *res);
int ck_cs_wire(struct Resource *res);
int post_send(struct Resource *res, int opcode);
int post_receive(struct Resource *res);
int poll_completion(struct Resource *res);
int com_op(struct Resource *res);
#endif /*ib.h*/

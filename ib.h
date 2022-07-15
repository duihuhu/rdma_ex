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
#define SYNC_MES	"sync"
#define IB_OP_SR	"SR"
#define IB_OP_RD	"READ"
#define IB_OP_WR	"WRITE"
#define IB_OP_CAS	"CAS"
struct Resource {
	struct	ibv_context	*ctx;
	struct	ibv_pd	*pd;
	struct	ibv_mr	*mr;
	struct	ibv_cq	*cq;
	struct	ibv_qp	*qp;
	struct	ibv_device_attr	dev_attr;
	struct	ibv_port_attr	port_attr;
	char	*ib_buf;
	int		ib_buf_size;
	int		sockfd;
	uint32_t	rkey;
	uint64_t	raddr;
};
struct QpInfo {
	uint16_t lid;
	uint32_t qp_num;
	uint32_t rkey;
	uint64_t raddr;
};
extern struct Resource res;

int init_ib();
static int post_send(int opcode);
static int poll_completion();

#endif /*ib.h*/

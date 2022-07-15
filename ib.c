/*************************************************************************
	> File Name: ib.c
	> Author: duihuhu
	> Mail: duihuhu@163.com 
	> Created Time: Mon 04 Jul 2022 02:01:13 PM UTC
 ************************************************************************/
#include <malloc.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "ib.h"
#include "config.h"
#include "sock.h"
struct Resource res;
int get_qp_info(int sockfd, struct QpInfo *qp_info)
{
	int ret;
	struct QpInfo tmp_qp_info;
	ret = sock_read(sockfd, (char *)&tmp_qp_info, sizeof(struct QpInfo));
	if (ret < 0) {
		fprintf(stdout, "sock read failed\n");
		return -1;
	}
	qp_info->lid = ntohs(tmp_qp_info.lid);
	qp_info->qp_num = ntohl(tmp_qp_info.qp_num);
	qp_info->rkey = ntohl(tmp_qp_info.rkey);
	qp_info->raddr = ntohll(tmp_qp_info.raddr);
	return 0;
}
int set_qp_info(int sockfd, struct QpInfo *qp_info)
{
	int ret;
	struct QpInfo tmp_qp_info;
	tmp_qp_info.lid = qp_info->lid;
	tmp_qp_info.qp_num = qp_info->qp_num;
	tmp_qp_info.rkey = qp_info->rkey;
	tmp_qp_info.raddr = qp_info->raddr;
	ret = sock_write(sockfd, (char *)&tmp_qp_info, sizeof(struct QpInfo));
	if (ret < 0) {
		fprintf(stdout, "sock write failed\n");
		return -1;
	}
	return 0;
}


int conv_qp_status(struct ibv_qp *qp, uint32_t qp_num ,uint16_t lid)
{
	{
		struct ibv_qp_attr attr = {
			.qp_state = IBV_QPS_INIT,
			.pkey_index = 0,
			.port_num = IB_PORT,
			.qp_access_flags = IBV_ACCESS_LOCAL_WRITE  | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_ATOMIC,
		};
		if (ibv_modify_qp(res.qp, &attr, IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS))
		{
			fprintf(stdout, "failed to convert to INIT");
			return -1;
		}
	}

	{
		struct ibv_qp_attr attr = {
			.qp_state = IBV_QPS_RTR,
			.path_mtu = IB_MTU,
			.dest_qp_num = qp_num,
			.rq_psn = 0,
			.max_dest_rd_atomic = 1,
			.min_rnr_timer = 12,
			.ah_attr.is_global = 0,
			.ah_attr.dlid = lid,
			.ah_attr.sl = IB_SL,
			.ah_attr.src_path_bits = 0,
			.ah_attr.port_num = IB_PORT,
		};
		if (ibv_modify_qp(res.qp, &attr, IBV_QP_STATE | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER | IBV_QP_AV)) {
			fprintf(stdout, "failed to convert to RTR");
			return -1;
		}
	}

	{
		struct ibv_qp_attr attr = {
			.qp_state = IBV_QPS_RTS,
			.sq_psn = 0,
			.timeout = 14,
			.retry_cnt = 7,
			.rnr_retry = 7,
			.max_rd_atomic = 1, 
		};
		if (ibv_modify_qp(res.qp, &attr, IBV_QP_STATE | IBV_QP_SQ_PSN | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT | IBV_QP_RNR_RETRY | IBV_QP_MAX_QP_RD_ATOMIC)) {
			fprintf(stdout, "failed to convert to RTS");
			return -1;
		}
	}
	return 0;
}

int ck_cs_wire() {
	int ret;
	char buf[10] = {'\0'};
	strcpy(buf, SYNC_MES);
	ret = sock_write(res.sockfd, (char *)buf, sizeof(SYNC_MES));
	if (ret < 0) {
		fprintf(stdout,"failed to write sync");
		return -1;
	}
	ret = sock_read(res.sockfd, (char *)buf, sizeof(SYNC_MES));
	if (ret < 0) {
		fprintf(stdout, "failed to read sync");
		return -1;
	}
	fprintf(stdout, "%s ck_cs_wire\n", buf);
	return 0;
}

int ex_qp_info()
{
	struct QpInfo local_info;
	struct QpInfo remote_info;
	memset(&remote_info, 0, sizeof(struct QpInfo));
	local_info.lid = htons(res.port_attr.lid);
	local_info.qp_num = htonl(res.qp->qp_num);
	local_info.rkey = htonl(res.mr->rkey);
	local_info.raddr = htonll((uintptr_t)res.ib_buf);
	int ret;
	if (!cfg.server_name) {
		ret = get_qp_info(res.sockfd, &remote_info);
		if (ret < 0) {
			fprintf(stdout, "failed to get qp info by server\n");
			return -1;
		}
		ret = set_qp_info(res.sockfd, &local_info);
		if (ret < 0) {
			fprintf(stdout, "failed to set qp info by server\n");
			return -1;
		}
	} else {
		ret = set_qp_info(res.sockfd, &local_info);
		if (ret < 0) {
			fprintf(stdout, "failed to set qp info by client\n");
			return -1;
		}
		ret= get_qp_info(res.sockfd, &remote_info);
		if (ret < 0) {
			fprintf(stdout, "failed to get qp info by client\n");
			return -1;
		}
	}
	res.rkey = remote_info.rkey;
	res.raddr = remote_info.raddr;
	fprintf(stdout, "local key 0x%x, local addr %p, remote key 0x%x, remote addr 0x%lx\n", res.mr->rkey, res.ib_buf, res.rkey, res.raddr);
	
	ret = conv_qp_status(res.qp, remote_info.qp_num, remote_info.lid);
	if (ret < 0) {
		fprintf(stdout, "failed to convert qp status \n");
		return -1;
	}
	ret = ck_cs_wire();
	if (ret < 0) {
		fprintf(stdout, "server-client is not ready");
		return -1;
	}
	return 0;
}

int init_ib()
{
	struct ibv_device	**ibv_devices = NULL;
	struct ibv_device	*ib_dev = NULL;
	int num_devices;
	memset(&res, 0, sizeof(struct Resource));
	ibv_devices = ibv_get_device_list(&num_devices);
	if (!num_devices) {
		fprintf(stdout, "no device\n");
		goto init_ib_exit;
	}
	int i;
	for (i=0; i<num_devices; ++i) {
		if (!cfg.dev_name) {
			ib_dev = ibv_devices[i];
			break;
		}
		if (!strcmp(cfg.dev_name, ibv_get_device_name(ibv_devices[i]))) {
			ib_dev = ibv_devices[i];
			break;
		}
	}
	if (!ib_dev) {
		fprintf(stdout, "no device name\n");
		goto init_ib_exit;
	}
	res.ctx = ibv_open_device(ib_dev);
	if (!res.ctx) {
		fprintf(stdout, "can't open device\n");
		goto init_ib_exit;
	}
	res.pd = ibv_alloc_pd(res.ctx);
	if (!res.pd) {
		fprintf(stdout, "alloc pd failed\n");
		goto init_ib_exit;
	}
	res.ib_buf_size = cfg.msg_size;
	// res.ib_buf = (char *) memalign(PAGE_SIZE, res.ib_buf_size);
	res.ib_buf = (char *) malloc(res.ib_buf_size);
	memset(res.buf, 0, res.ib_buf_size);
	if (!res.ib_buf) {
		fprintf(stdout, "alloc buffer failed\n");
		goto init_ib_exit;
	}
	int mflags = 0;
	mflags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE;
	res.mr = ibv_reg_mr(res.pd, (void *)res.ib_buf, res.ib_buf_size, mflags);
	if (!res.mr) {
		fprintf(stdout, "alloc mr failed\n");
		goto init_ib_exit;
	}
	int rc;
	rc = ibv_query_device(res.ctx, &res.dev_attr);
	if (rc) {
		fprintf(stdout, "failed to query device\n ");
		goto init_ib_exit;
	}
	res.cq = ibv_create_cq(res.ctx, res.dev_attr.max_cqe, NULL, NULL, 0);
	if (!res.cq) {
		fprintf(stdout, "failed create cq\n");
		goto init_ib_exit;
	}
	struct ibv_qp_init_attr qp_init_attr;
	memset(&qp_init_attr, 0, sizeof(qp_init_attr));
	qp_init_attr.send_cq = res.cq;
	qp_init_attr.recv_cq = res.cq;
	qp_init_attr.qp_type = IBV_QPT_RC;
	qp_init_attr.cap.max_send_wr = 10;
	qp_init_attr.cap.max_recv_wr = 10;
	qp_init_attr.cap.max_send_sge = 1;
	qp_init_attr.cap.max_recv_sge = 1;
	res.qp = ibv_create_qp(res.pd, &qp_init_attr);
	if (!res.qp) {
		fprintf(stdout, "failed create qp\n");
		goto init_ib_exit;
	}
	int ret;
	ret = ibv_query_port(res.ctx, IB_PORT, &res.port_attr);
	if (ret) {
		fprintf(stdout, "query port info failed\n");
		return -1;
	}
	fprintf(stdout, "mr was register addr=%p, lkey=0x%x, rkey=0x%x, flags=0x%x\n", res.ib_buf, res.mr->lkey, res.mr->rkey, mflags);
	if (!cfg.server_name) {
		ret = socket_connect(NULL, cfg.tcp_port);
		if (ret < 0) {
			fprintf(stdout, "failed to establish server\n");
			goto init_ib_exit;
		}
	} else {
		ret = socket_connect(cfg.server_name, cfg.tcp_port);
		if (ret < 0) {
			fprintf(stdout, "failed to establish connect\n");
			goto init_ib_exit;
		}
	}
	ret = ex_qp_info();
	if (ret < 0) {
		fprintf(stdout, "failed ex qp info\n");
		goto init_ib_exit;
	}
	return 0;
init_ib_exit:
	if (res.qp)
		ibv_destroy_qp(res.qp);
	if (res.cq)
		ibv_destroy_cq(res.cq);
	if (res.mr)
		ibv_dereg_mr(res.mr);
	if (res.ib_buf)
		free(res.ib_buf);
	if (res.pd)
		ibv_dealloc_pd(res.pd);
	if (res.ctx)
		ibv_close_device(res.ctx);
	if (ibv_devices)
		ibv_free_device_list(ibv_devices);
	return -1;
}


int poll_completion()
{
	struct ibv_wc wc;
	unsigned long start_time_msec;
	unsigned long cur_time_msec;
	struct timeval cur_time;
	int poll_result;
	int rc = 0;
	/* poll the completion for a while before giving up of doing it .. */
	gettimeofday(&cur_time, NULL);
	start_time_msec = (cur_time.tv_sec * 1000) + (cur_time.tv_usec / 1000);
	do
	{
		poll_result = ibv_poll_cq(res.cq, 1, &wc);
		gettimeofday(&cur_time, NULL);
		cur_time_msec = (cur_time.tv_sec * 1000) + (cur_time.tv_usec / 1000);
	} while ((poll_result == 0) && ((cur_time_msec - start_time_msec) < MAX_POLL_CQ_TIMEOUT));
	if (poll_result < 0)
	{
		/* poll CQ failed */
		fprintf(stderr, "poll CQ failed\n");
		rc = 1;
	}
	else if (poll_result == 0)
	{ /* the CQ is empty */
		fprintf(stderr, "completion wasn't found in the CQ after timeout\n");
		rc = 1;
	}
	else
	{
		/* CQE found */
		fprintf(stdout, "completion was found in CQ with status 0x%x\n", wc.status);
		/* check the completion status (here we don't care about the completion opcode */
		if (wc.status != IBV_WC_SUCCESS)
		{
			fprintf(stderr, "got bad completion with status: 0x%x, vendor syndrome: 0x%x\n", wc.status,
					wc.vendor_err);
			rc = 1;
		}
	}
	return rc;
}

int post_send(int opcode)
{
	struct ibv_send_wr sr;
	struct ibv_sge sge;
	struct ibv_send_wr *bad_wr = NULL;
	int rc;
	/* prepare the scatter/gather entry */
	memset(&sge, 0, sizeof(sge));
	sge.addr = (uintptr_t)res.ib_buf;
	sge.length = res.ib_buf_size;
	sge.lkey = res.mr->lkey;
	/* prepare the send work request */
	memset(&sr, 0, sizeof(sr));
	sr.next = NULL;
	sr.wr_id = 0;
	sr.sg_list = &sge;
	sr.num_sge = 1;
	sr.opcode = opcode;
	sr.send_flags = IBV_SEND_SIGNALED;
	if (opcode != IBV_WR_SEND)
	{
		sr.wr.rdma.remote_addr = res.raddr;
		sr.wr.rdma.rkey = res.rkey;
	}
	/* there is a Receive Request in the responder side, so we won't get any into RNR flow */
	rc = ibv_post_send(res.qp, &sr, &bad_wr);
	if (rc)
		fprintf(stderr, "failed to post SR\n");
	else
	{
		switch (opcode)
		{
		case IBV_WR_SEND:
			fprintf(stdout, "Send Request was posted\n");
			break;
		case IBV_WR_RDMA_READ:
			fprintf(stdout, "RDMA Read Request was posted\n");
			break;
		case IBV_WR_RDMA_WRITE:
			fprintf(stdout, "RDMA Write Request was posted\n");
			break;
		default:
			fprintf(stdout, "Unknown Request was posted\n");
			break;
		}
	}
	return rc;
}

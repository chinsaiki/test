/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2016 Intel Corporation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_eal.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

static volatile bool rtoax_force_quit;

/* MAC updating enabled by default */
static int rtoax_mac_updating = 1;

#define RTE_LOGTYPE_L2FWD RTE_LOGTYPE_USER1

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN_US 100 /* TX drain every ~100us */
#define MEMPOOL_CACHE_SIZE 256

/*
 * Configurable number of RX/TX ring descriptors
 */
#define RTE_TEST_RX_DESC_DEFAULT 1024
#define RTE_TEST_TX_DESC_DEFAULT 1024
static uint16_t rtoax_nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
static uint16_t rtoax_nb_txd = RTE_TEST_TX_DESC_DEFAULT;

/* ethernet addresses of ports */
static struct rte_ether_addr rtoax_l2fwd_ports_eth_addr[RTE_MAX_ETHPORTS];

/* mask of enabled ports */
static uint32_t rtoax_l2fwd_enabled_port_mask = 0;

/* list of enabled ports */
static uint32_t rtoax_l2fwd_dst_ports[RTE_MAX_ETHPORTS];

static unsigned int rtoax_l2fwd_rx_queue_per_lcore = 1;

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16
struct lcore_queue_conf {
	unsigned n_rx_port;
	unsigned rx_port_list[MAX_RX_QUEUE_PER_LCORE];
} __rte_cache_aligned;
struct lcore_queue_conf rtoax_lcore_queue_cfg[RTE_MAX_LCORE];

static struct rte_eth_dev_tx_buffer *rtoax_l2fwd_tx_buffer[RTE_MAX_ETHPORTS];

static struct rte_eth_conf port_conf = {
	.rxmode = {
		.split_hdr_size = 0,
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

struct rte_mempool * rtoax_l2fwd_pktmbuf_pool = NULL;

/* Per-port statistics struct */
struct l2fwd_port_statistics {
	uint64_t tx;
	uint64_t rx;
	uint64_t dropped;
} __rte_cache_aligned;
struct l2fwd_port_statistics rtoax_port_statistics[RTE_MAX_ETHPORTS];

#define MAX_TIMER_PERIOD 86400 /* 1 day max */
/* A tsc-based timer responsible for triggering statistics printout */
static uint64_t rtoax_timer_period = 1; /* default period is 1 seconds */
static uint64_t rtoax_timer_interval = 0;

/* Print out statistics on packets dropped */
static void
rtoax_l2fwd_print_stats(void)
{
    //return;
	static int filter_cnt=0;
    if((filter_cnt % 20) !=0) {
        filter_cnt++;
        return;
    }
    filter_cnt=0;

	uint64_t total_packets_dropped, total_packets_tx, total_packets_rx;
	unsigned portid;
    static uint64_t TX_Mbp_after[RTE_MAX_ETHPORTS] = {0};
    static uint64_t RX_Mbp_after[RTE_MAX_ETHPORTS] = {0};
    
	total_packets_dropped = 0;
	total_packets_tx = 0;
	total_packets_rx = 0;

#ifdef __rtoax_debug
	const char clr[] = { 27, '[', '2', 'J', '\0' };
	const char topLeft[] = { 27, '[', '1', ';', '1', 'H','\0' };

		/* Clear screen and move to top left */
	printf("%s%s", clr, topLeft);
#endif

	printf("\nPort statistics ====================================");

	for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++) {
		/* skip disabled ports */
		if ((rtoax_l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;
		printf("\nStatistics for port %u ------------------------------"
			   "\nPackets sent: %24"PRIu64
			   "\nPackets received: %20"PRIu64
			   "\nPackets dropped: %21"PRIu64,
			   portid,
			   rtoax_port_statistics[portid].tx,
			   rtoax_port_statistics[portid].rx,
			   rtoax_port_statistics[portid].dropped);
        
        printf("\nPackets buf size: %20"PRIu16, rtoax_l2fwd_tx_buffer[portid]->size);
//        printf("\nPackets buf len: %21"PRIu16, rtoax_l2fwd_tx_buffer[portid]->length);
        uint64_t tx_mbp = rtoax_l2fwd_tx_buffer[portid]->size*rtoax_port_statistics[portid].tx*8/1024/1024;
        uint64_t rx_mbp = rtoax_l2fwd_tx_buffer[portid]->size*rtoax_port_statistics[portid].rx*8/1024/1024;
        
        printf("\nPackets send rate: %19"PRIu64" Mbps", (tx_mbp - TX_Mbp_after[portid])/rtoax_timer_interval);
        printf("\nPackets recv rate: %19"PRIu64" Mbps", (rx_mbp - RX_Mbp_after[portid])/rtoax_timer_interval);
        TX_Mbp_after[portid] = tx_mbp;
        RX_Mbp_after[portid] = rx_mbp;

		total_packets_dropped += rtoax_port_statistics[portid].dropped;
		total_packets_tx += rtoax_port_statistics[portid].tx;
		total_packets_rx += rtoax_port_statistics[portid].rx;
	}
    
	printf("\nAggregate statistics ==============================="
		   "\nTotal packets sent: %18"PRIu64
		   "\nTotal packets received: %14"PRIu64
		   "\nTotal packets dropped: %15"PRIu64,
		   total_packets_tx,
		   total_packets_rx,
		   total_packets_dropped);
	printf("\n====================================================\n"); 
    
    //Port statistics ====================================
    //Statistics for port 0 ------------------------------
    //Packets sent:                841020320
    //Packets received:            839716760
    //Packets dropped:                     0
    //Packets buf size:                   32
    //Packets send rate:                2593 Mbps
    //Packets recv rate:                2592 Mbps
    //Statistics for port 1 ------------------------------
    //Packets sent:                839716760
    //Packets received:            841020320
    //Packets dropped:                     0
    //Packets buf size:                   32
    //Packets send rate:                2592 Mbps
    //Packets recv rate:                2593 Mbps
    //Aggregate statistics ===============================
    //Total packets sent:         1680737080
    //Total packets received:     1680737080
    //Total packets dropped:               0
    //====================================================


    
}

/* 获取MAC地址 */
static void rtoax_l2fwd_mac_updating(struct rte_mbuf *m, unsigned dest_portid)
{
	struct rte_ether_hdr *eth;
	void *tmp;

    /* 从mbuf中获取ether头 */
	eth = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

	/* 02:00:00:00:00:xx *//* 目的MAC地址 */
	tmp = &eth->d_addr.addr_bytes[0];
	*((uint64_t *)tmp) = 0x000000000002 + ((uint64_t)dest_portid << 40);

	/* src addr *//* 源MAC地址 */
	rte_ether_addr_copy(&rtoax_l2fwd_ports_eth_addr[dest_portid], &eth->s_addr);
}

/* 发送一个mac报文 */
static void rtoax_l2fwd_simple_forward(struct rte_mbuf *m, unsigned portid)
{
	unsigned dst_port;
	int sent;
	struct rte_eth_dev_tx_buffer *buffer;

    /* 获取目的端口 */
	dst_port = rtoax_l2fwd_dst_ports[portid];

    /* 默认开启 */
	if (rtoax_mac_updating)
		rtoax_l2fwd_mac_updating(m, dst_port);

    /* 往发送队列里写数据，先获取发送队列buffer */
	buffer = rtoax_l2fwd_tx_buffer[dst_port];

    /* 向发送buffer中写 ， 将 m 添加到buffer 的 rte_mbuf中， 然后调用flush*/
	sent = rte_eth_tx_buffer(dst_port, /* 目的端口 */
	                         0, /* 队列ID */
	                         buffer, /* 队列buffer，启动包含了rte_mbuf结构， */
	                         m);/* 准备发送的rte_mbuf */
    /* 统计发送数据 */
	if (sent)
		rtoax_port_statistics[dst_port].tx += sent;
}

/* main processing loop */
/*  主进程的循环  */
static void rtoax_l2fwd_main_loop(void)
{
	struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
	struct rte_mbuf *m;
	int sent;
	unsigned lcore_id;
	uint64_t prev_tsc, diff_tsc, cur_tsc, timer_tsc;
	unsigned i, j, portid, nb_rx;
	struct lcore_queue_conf *qconf;

    /* 时间分辨率   */
	const uint64_t drain_tsc = (rte_get_tsc_hz() + US_PER_S - 1) / US_PER_S * BURST_TX_DRAIN_US;
	struct rte_eth_dev_tx_buffer *buffer;

	prev_tsc = 0;
	timer_tsc = 0;

	lcore_id = rte_lcore_id();/*  核心ID   */
	qconf = &rtoax_lcore_queue_cfg[lcore_id];/*  lcore配置   */

    /*  接收队列不能为0    */
	if (qconf->n_rx_port == 0) {
		RTE_LOG(INFO, L2FWD, "lcore %u has nothing to do\n", lcore_id);
		return;
	}

	RTE_LOG(INFO, L2FWD, "entering main loop on lcore %u\n", lcore_id);

    /*  轮询接收端口   */
	for (i = 0; i < qconf->n_rx_port; i++) {
		portid = qconf->rx_port_list[i];
		RTE_LOG(INFO, L2FWD, " -- lcoreid=%u portid=%u\n", lcore_id, portid);
	}

    /* 进行队列的轮询 */
	while (!rtoax_force_quit) {
        /* 获取当前时间 */
		cur_tsc = rte_rdtsc();

		/*
		 * TX burst queue drain
		 *//* 计算时差 */
		diff_tsc = cur_tsc - prev_tsc;

        /*  这里成百上千个包才会进入这个if分支内执行一次    */
		if (unlikely(diff_tsc > drain_tsc)) {   /* 时间差大于分辨率，这是低概率事件 */

            /*  轮询接收端口   */
			for (i = 0; i < qconf->n_rx_port; i++) {

				portid = rtoax_l2fwd_dst_ports[qconf->rx_port_list[i]];
				buffer = rtoax_l2fwd_tx_buffer[portid];

                /* flush 发送队列 */
				sent = rte_eth_tx_buffer_flush(portid, 0, buffer);
				if (sent)
					rtoax_port_statistics[portid].tx += sent;
			}

			/* if timer is enabled */
			if (rtoax_timer_period > 0) {

				/* advance the timer */
				timer_tsc += diff_tsc;

				/* if timer has reached its timeout */
				if (unlikely(timer_tsc >= rtoax_timer_period)) {

					/* do this only on master core */
					if (lcore_id == rte_get_master_lcore()) {

                        /* 如果是主lcore，打印统计信息 */
						rtoax_l2fwd_print_stats();

						/* reset the timer */
						timer_tsc = 0;
					}
				}
			}

			prev_tsc = cur_tsc;
		}

		/*
		 * Read packet from RX queues 从队列中读取数据
		 *//*  发送接收数据    */
		for (i = 0; i < qconf->n_rx_port; i++) {

			portid = qconf->rx_port_list[i];

            /* 接收 */
			nb_rx = rte_eth_rx_burst(portid, 0, pkts_burst, MAX_PKT_BURST);

			rtoax_port_statistics[portid].rx += nb_rx;

			for (j = 0; j < nb_rx; j++) {
				m = pkts_burst[j];

                /* 进行预取 ((void *)((char *)rte_mbuf->buf_addr + rte_mbuf->data_off)) */
				rte_prefetch0(rte_pktmbuf_mtod(m, void *));

                /* forward */
				rtoax_l2fwd_simple_forward(m, portid);
			}
		}
	}
}

static int rtoax_l2fwd_launch_one_lcore(__rte_unused void *dummy)
{
    /* 循环 */
	rtoax_l2fwd_main_loop();
	return 0;
}

/* display usage */
static void rtoax_l2fwd_usage(const char *prgname)
{
	printf("%s [EAL options] -- -p PORTMASK [-q NQ]\n"
	       "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
	       "  -q NQ: number of queue (=ports) per lcore (default is 1)\n"
		   "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 10 default, 86400 maximum)\n"
		   "  --[no-]mac-updating: Enable or disable MAC addresses updating (enabled by default)\n"
		   "      When enabled:\n"
		   "       - The source MAC address is replaced by the TX port MAC address\n"
		   "       - The destination MAC address is replaced by 02:00:00:00:00:TX_PORT_ID\n",
	       prgname);
}

static int rtoax_l2fwd_parse_portmask(const char *portmask)
{
    /* 解析16进制数据 */
	char *end = NULL;
	unsigned long pm;

	/* parse hexadecimal string */
	pm = strtoul(portmask, &end, 16);
	if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
		return -1;

	if (pm == 0)
		return -1;

	return pm;
}

static unsigned int rtoax_l2fwd_parse_nqueue(const char *q_arg)
{
    /* 解析10进制数据 */
	char *end = NULL;
	unsigned long n;

	/* parse hexadecimal string */
	n = strtoul(q_arg, &end, 10);
	if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
		return 0;
	if (n == 0)
		return 0;
	if (n >= MAX_RX_QUEUE_PER_LCORE)
		return 0;

	return n;
}

static int rtoax_l2fwd_parse_timer_period(const char *q_arg)
{
	char *end = NULL;
	int n;

	/* parse number string */
	n = strtol(q_arg, &end, 10);
	if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
		return -1;
	if (n >= MAX_TIMER_PERIOD)
		return -1;

	return n;
}

static const char short_options[] =
	"p:"  /* portmask */
	"q:"  /* number of queues */
	"T:"  /* timer period */
	;

#define CMD_LINE_OPT_MAC_UPDATING "mac-updating"
#define CMD_LINE_OPT_NO_MAC_UPDATING "no-mac-updating"

enum {
	/* long options mapped to a short option */

	/* first long only option value must be >= 256, so that we won't
	 * conflict with short options */
	CMD_LINE_OPT_MIN_NUM = 256,
};

static const struct option lgopts[] = {
	{ CMD_LINE_OPT_MAC_UPDATING, no_argument, &rtoax_mac_updating, 1},
	{ CMD_LINE_OPT_NO_MAC_UPDATING, no_argument, &rtoax_mac_updating, 0},
	{NULL, 0, 0, 0}
};

/* Parse the argument given in the command line of the application */
static int rtoax_l2fwd_parse_args(int argc, char **argv)
{
	int opt, ret, timer_secs;
	char **argvopt;
	int option_index;
	char *prgname = argv[0];

	argvopt = argv;

	while ((opt = getopt_long(argc, argvopt, short_options,
				  lgopts, &option_index)) != EOF) {

		switch (opt) {
		/* portmask */
		case 'p':/* 端口掩码 */
			rtoax_l2fwd_enabled_port_mask = rtoax_l2fwd_parse_portmask(optarg);
			if (rtoax_l2fwd_enabled_port_mask == 0) {
				printf("invalid portmask\n");
				rtoax_l2fwd_usage(prgname);
				return -1;
			}
			break;

		/* nqueue */
		case 'q':/* 接收队列数 */
			rtoax_l2fwd_rx_queue_per_lcore = rtoax_l2fwd_parse_nqueue(optarg);
			if (rtoax_l2fwd_rx_queue_per_lcore == 0) {
				printf("invalid queue number\n");
				rtoax_l2fwd_usage(prgname);
				return -1;
			}
			break;

		/* timer period */
		case 'T':/* 定时器周期 */
			timer_secs = rtoax_l2fwd_parse_timer_period(optarg);
			if (timer_secs < 0) {
				printf("invalid timer period\n");
				rtoax_l2fwd_usage(prgname);
				return -1;
			}
			rtoax_timer_period = timer_secs;/*  */
            rtoax_timer_interval = rtoax_timer_period;
			break;

		/* long options */
		case 0:
			break;

		default:
			rtoax_l2fwd_usage(prgname);
			return -1;
		}
	}

	if (optind >= 0)
		argv[optind-1] = prgname;

	ret = optind-1;
	optind = 1; /* reset getopt lib */
	return ret;
}

#ifdef __rtoax_debug__
struct rte_eth_link {
	uint32_t link_speed;        /**< ETH_SPEED_NUM_ */
	uint16_t link_duplex  : 1;  /**< ETH_LINK_[HALF/FULL]_DUPLEX */
	uint16_t link_autoneg : 1;  /**< ETH_LINK_[AUTONEG/FIXED] */
	uint16_t link_status  : 1;  /**< ETH_LINK_[DOWN/UP] */
} __rte_aligned(8);      /**< aligned for atomic64 read/write */
/* Utility constants */
#define ETH_LINK_HALF_DUPLEX 0 /**< Half-duplex connection (see link_duplex). */
#define ETH_LINK_FULL_DUPLEX 1 /**< Full-duplex connection (see link_duplex). */
#define ETH_LINK_DOWN        0 /**< Link is down (see link_status). */
#define ETH_LINK_UP          1 /**< Link is up (see link_status). */
#define ETH_LINK_FIXED       0 /**< No autonegotiation (see link_autoneg). */
#define ETH_LINK_AUTONEG     1 /**< Autonegotiated (see link_autoneg). */

#endif

/* Check the link status of all ports in up to 9s, and print them finally */
static void rtoax_check_all_ports_link_status(uint32_t port_mask)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */

	uint16_t portid;
	uint8_t count, all_ports_up, print_flag = 0;
	struct rte_eth_link link;
	int ret;

	printf("\nChecking link status...\n");
	fflush(stdout);
	for (count = 0; count <= MAX_CHECK_TIME; count++) {
		if (rtoax_force_quit)
			return;
		all_ports_up = 1;

        /*  轮询所有设备 */
		RTE_ETH_FOREACH_DEV(portid) {
			if (rtoax_force_quit)
				return;
			if ((port_mask & (1 << portid)) == 0)
				continue;
			memset(&link, 0, sizeof(link));

            /*  获取端口link状态 */
			ret = rte_eth_link_get_nowait(portid, &link);
			if (ret < 0) {
				all_ports_up = 0;
				if (print_flag == 1)
					printf("Port %u link get failed: %s\n", portid, rte_strerror(-ret));
				continue;
			}
			/* print link status if flag set */
			if (print_flag == 1) {
				if (link.link_status)
					printf("Port%d Link Up. Speed %u Mbps - %s, AutoNeg - %s\n", portid, link.link_speed,
                				(link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
                					("full-duplex") : ("half-duplex"),
                				(link.link_autoneg == ETH_LINK_FIXED) ? /* 是否 自动协商 */
                					("No autonegotiation") : ("Autonegotiated"));
				else
					printf("Port %d Link Down\n", portid);
				continue;
			}
			/* clear all_ports_up flag if any link down */
			if (link.link_status == ETH_LINK_DOWN) {
				all_ports_up = 0;
				break;
			}
		}
		/* after finally printing all link status, get out */
		if (print_flag == 1)
			break;

		if (all_ports_up == 0) {
			printf(".");
			fflush(stdout);
			rte_delay_ms(CHECK_INTERVAL);
		}

		/* set the print_flag if all ports up or timeout */
		if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
			print_flag = 1;
            printf("\nChecking link status...done \n");
		}
	}
}

/* 监听信号，置位退出变量 */
static void rtoax_l2fwd_signal_handler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM) {
		printf("\n\nSignal %d received, preparing to exit...\n",
				signum);
		rtoax_force_quit = true;
	}
}

int main(int argc, char **argv)
{
	struct lcore_queue_conf *qconf; /* j接收端口，接收端口列表 */
	int ret;
	uint16_t nb_ports;
	uint16_t nb_ports_available = 0;
	uint16_t portid, last_port;
	unsigned lcore_id, rx_lcore_id;
	unsigned nb_ports_in_mask = 0;
	unsigned int nb_lcores = 0;
	unsigned int nb_mbufs;

	/* init EAL *//* 初始化 DPDK EAL环境 */
	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
	argc -= ret;
	argv += ret;

    /* 信号回调函数中将该值设置为true */
	rtoax_force_quit = false;

    /* 监听信号 */
	signal(SIGINT, rtoax_l2fwd_signal_handler);
	signal(SIGTERM, rtoax_l2fwd_signal_handler);

	/* parse application arguments (after the EAL ones) */
	ret = rtoax_l2fwd_parse_args(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Invalid L2FWD arguments\n");

    /* 更新MAC地址 */
	printf("MAC updating %s\n", rtoax_mac_updating ? "enabled" : "disabled");

    /* 时间间隔 */
    rtoax_timer_interval = rtoax_timer_period;

	/* convert to number of cycles */
	rtoax_timer_period *= rte_get_timer_hz();/* 定时器超时时间 */

    /* 可用网口数量，内部轮询 RTE_ETH_FOREACH_DEV 全局变量 rte_eth_devices */
	nb_ports = rte_eth_dev_count_avail();
	if (nb_ports == 0)
		rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

	/* check port mask to possible port mask *//* 检测参数有效性 */
	if (rtoax_l2fwd_enabled_port_mask & ~((1 << nb_ports) - 1))
		rte_exit(EXIT_FAILURE, "Invalid portmask; possible (0x%x)\n", (1 << nb_ports) - 1);

	/* reset rtoax_l2fwd_dst_ports *//* 重置网口=0 */
	for (portid = 0; portid < RTE_MAX_ETHPORTS; portid++)
		rtoax_l2fwd_dst_ports[portid] = 0; 
    
	last_port = 0;

	/*
	 * Each logical core is assigned a dedicated TX queue on each port.
	 *//* 遍历网卡设备 */
	RTE_ETH_FOREACH_DEV(portid) {
		/* skip ports that are not enabled *//* 跳过mask没设置的网口 */
		if ((rtoax_l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;

        /* 有效port设为不同的网卡：聪明之举，还好LZ有两个网卡 */
		if (nb_ports_in_mask % 2) {/* 如果是计数为奇数 */
			rtoax_l2fwd_dst_ports[portid] = last_port;/*  */
			rtoax_l2fwd_dst_ports[last_port] = portid;
		}
		else/* 偶数 */
			last_port = portid;/*  */

        /* 网口计数 */
		nb_ports_in_mask++;
	}
    
    /*  需要有偶数个port */
	if (nb_ports_in_mask % 2) {
		printf("Notice: odd number of ports in portmask.\n");
		rtoax_l2fwd_dst_ports[last_port] = last_port;
	}

	rx_lcore_id = 0;/* 接收core id初始化 */
	qconf = NULL;

	/* Initialize the port/queue configuration of each logical core */
    /* 为每个逻辑核初始化端口/队列配置  */
	RTE_ETH_FOREACH_DEV(portid) {
		/* skip ports that are not enabled *//* 确定该端口可用  */
		if ((rtoax_l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;

        /* 核可用  */
		/* get the lcore_id for this port *//* 为port获取lcore_id核心 */
		while (rte_lcore_is_enabled(rx_lcore_id) == 0 ||
		       rtoax_lcore_queue_cfg[rx_lcore_id].n_rx_port == rtoax_l2fwd_rx_queue_per_lcore) {/*  接收队列 */
		       
			rx_lcore_id++;
			if (rx_lcore_id >= RTE_MAX_LCORE)/* 确定核可用  */
				rte_exit(EXIT_FAILURE, "Not enough cores\n");
		}
        /* 队列配置 */
		if (qconf != &rtoax_lcore_queue_cfg[rx_lcore_id]) {
			/* Assigned a new logical core in the loop above. */
			qconf = &rtoax_lcore_queue_cfg[rx_lcore_id];
			nb_lcores++;
		}
        
        /* 接收队列表  */
		qconf->rx_port_list[qconf->n_rx_port] = portid;
		qconf->n_rx_port++;
		printf("Lcore %u: RX port %u\n", rx_lcore_id, portid);
	}

    /* mbufs大小  */
	nb_mbufs = RTE_MAX(nb_ports * (rtoax_nb_rxd + rtoax_nb_txd + MAX_PKT_BURST +
		                nb_lcores * MEMPOOL_CACHE_SIZE), 8192U);

	/* create the mbuf pool Mbuf Pool Initialization */
        /*  mbufs内存池 https://rtoax.blog.csdn.net/article/details/109775955 */
	rtoax_l2fwd_pktmbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", nb_mbufs,
		MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
		rte_socket_id());
	if (rtoax_l2fwd_pktmbuf_pool == NULL)
		rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    /* 初始化所有port */
	/* Initialise each port */
	RTE_ETH_FOREACH_DEV(portid) {
		struct rte_eth_rxconf rxq_conf;
		struct rte_eth_txconf txq_conf;
		struct rte_eth_conf local_port_conf = port_conf;
		struct rte_eth_dev_info dev_info;

		/* skip ports that are not enabled */
		if ((rtoax_l2fwd_enabled_port_mask & (1 << portid)) == 0) {
			printf("Skipping disabled port %u\n", portid);
			continue;
		}
		nb_ports_available++;/* 可用port计数 */

		/* init port */
		printf("Initializing port %u... \n", portid);
		fflush(stdout);

        /* 获取网口信息 */
		ret = rte_eth_dev_info_get(portid, &dev_info);
		if (ret != 0)
			rte_exit(EXIT_FAILURE, "Error during getting device (port %u) info: %s\n", portid, strerror(-ret));

		if (dev_info.tx_offload_capa & DEV_TX_OFFLOAD_MBUF_FAST_FREE)
			local_port_conf.txmode.offloads |= DEV_TX_OFFLOAD_MBUF_FAST_FREE;

        /* 配置设备，接收队列和发送队列个数 */
		ret = rte_eth_dev_configure(portid, 1, 1, &local_port_conf);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n", ret, portid);

        /* 发送和接收队列目的 */
		ret = rte_eth_dev_adjust_nb_rx_tx_desc(portid, &rtoax_nb_rxd, &rtoax_nb_txd);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "Cannot adjust number of descriptors: err=%d, port=%u\n", ret, portid);

        /* 获取端口的MAC地址 */
		ret = rte_eth_macaddr_get(portid, &rtoax_l2fwd_ports_eth_addr[portid]);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "Cannot get MAC address: err=%d, port=%u\n", ret, portid);

		/* init one RX queue */
		fflush(stdout);

        /* 接收配置 */
		rxq_conf = dev_info.default_rxconf;/* 默认值 */
		rxq_conf.offloads = local_port_conf.rxmode.offloads; //接收队列分流

        /* 接收队列建立 */
		ret = rte_eth_rx_queue_setup(portid,  /* 端口号 */
		                             0,      /* 接收队列号 */
		                             rtoax_nb_rxd, /* 接收描述符个数 */
            					     rte_eth_dev_socket_id(portid), /* 设备属于哪个socket */
            					     &rxq_conf, /* 配置参数 */
            					     rtoax_l2fwd_pktmbuf_pool);    /* mbufs内存池 */
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup:err=%d, port=%u\n", ret, portid);

		/* init one TX queue on each port */
		fflush(stdout);

         /* 发送队列配置 */
		txq_conf = dev_info.default_txconf;  /* 发送配置默认值 */
		txq_conf.offloads = local_port_conf.txmode.offloads; /* 发送队列分流 */
		ret = rte_eth_tx_queue_setup(portid,  /* 端口 */
                                     0,      /* 发送队列号 */
                                     rtoax_nb_txd,   /* 发送描述符个数 */
                      				 rte_eth_dev_socket_id(portid), /* socket号 */
                      				 &txq_conf); /* 发送队列配置 */
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n", ret, portid);

		/* Initialize TX buffers */ /* 分配发送buffer内存 */
		rtoax_l2fwd_tx_buffer[portid] = rte_zmalloc_socket("rtoax_l2fwd_tx_buffer", /* 该字符串最终未使用 */
                                				RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST),  /* rte_mbuf*MAX_PKT_BURST+ rte_eth_dev_tx_buffer */
                                				0, /* 字节对齐 */
                                				rte_eth_dev_socket_id(portid));/* socket ID  */
		if (rtoax_l2fwd_tx_buffer[portid] == NULL)
			rte_exit(EXIT_FAILURE, "Cannot allocate buffer for tx on port %u\n", portid);

        /* 发送buffer初始化 */
		rte_eth_tx_buffer_init(rtoax_l2fwd_tx_buffer[portid], MAX_PKT_BURST);

        /* 发送失败回调函数 */
		ret = rte_eth_tx_buffer_set_err_callback(rtoax_l2fwd_tx_buffer[portid], /* 发送buffer  */
                                  				rte_eth_tx_buffer_count_callback,/* 发送失败回调函数 */
                                  				&rtoax_port_statistics[portid].dropped);/* 发送失败，包被drop */
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "Cannot set error callback for tx buffer on port %u\n", portid);

        /* 设置数据包类型，这里为unknown */
		ret = rte_eth_dev_set_ptypes(portid, RTE_PTYPE_UNKNOWN, NULL, 0);
		if (ret < 0)
			printf("Port %u, Failed to disable Ptype parsing\n", portid);
        
		/* Start device *//* 启动设备 */
		ret = rte_eth_dev_start(portid);
		if (ret < 0)
			rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n", ret, portid);

		printf("Initializing port %u... done: \n", portid);

        /* 开启混杂模式 */
		ret = rte_eth_promiscuous_enable(portid);
		if (ret != 0)
			rte_exit(EXIT_FAILURE, "rte_eth_promiscuous_enable:err=%s, port=%u\n", rte_strerror(-ret), portid);

        /* 打印一些信息 */
        printf("\033[1;%dm", 31+portid%4);

        //B4:96:91:6D:B4:CC
        //B4:96:91:6D:B4:CE
		printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
				portid,
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[0],
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[1],
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[2],
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[3],
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[4],
				rtoax_l2fwd_ports_eth_addr[portid].addr_bytes[5]);

        printf("Port %u, Driver %s.\n", portid, dev_info.driver_name);


        printf("\033[m");


		/* initialize port stats */
		memset(&rtoax_port_statistics, 0, sizeof(rtoax_port_statistics));
	}

    /* 如果端口都不可用，退出 */
	if (!nb_ports_available) {
		rte_exit(EXIT_FAILURE, "All available ports are disabled. Please set portmask.\n");
	}

    /* 检测所有端口的连接状态 */
	rtoax_check_all_ports_link_status(rtoax_l2fwd_enabled_port_mask);

	ret = 0;
    
	/* launch per-lcore init on every lcore */
    /*  执行lcore */
	rte_eal_mp_remote_launch(rtoax_l2fwd_launch_one_lcore, NULL, CALL_MASTER);

    /*  轮询slave */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
	    /*  等待结束 */
		if (rte_eal_wait_lcore(lcore_id) < 0) {
			ret = -1;
			break;
		}
	}
    /*  轮询所有设备 */
	RTE_ETH_FOREACH_DEV(portid) {
		if ((rtoax_l2fwd_enabled_port_mask & (1 << portid)) == 0)
			continue;
		printf("Closing port %d...", portid);
		rte_eth_dev_stop(portid);
		rte_eth_dev_close(portid);
		printf(" Done\n");
	}
	printf("Bye...\n");

	return ret;
}

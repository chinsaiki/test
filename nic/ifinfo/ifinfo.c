/**
 *  获取网口信息，IP地址，MAC地址，状态，带宽等
 *  
 *  日期：2020年10月16日
 *  作者：荣涛
 */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include "ifinfo.h"

#define TIMER_TICK_SEC  1
#define MAX_IF_NR   64

struct if_rx_tx_bytes {
    unsigned int if_idx;    //接口索引号
    char if_name[32];       //接口名 如：eth0

    unsigned long rx_bytes_prev;
    unsigned long rx_bytes_next;
    unsigned long rx_Bps;
    
    unsigned long tx_bytes_prev;
    unsigned long tx_bytes_next;
    unsigned long tx_Bps;
    
    unsigned int timer_interval_sec;
} *if_realtime_speed_entry[MAX_IF_NR] = {NULL};


static pthread_t tid_get_realtime_speed;
static pthread_rwlock_t realtime_speed_lock = PTHREAD_RWLOCK_INITIALIZER;

static struct if_rx_tx_bytes* new_if_rx_tx_entry(unsigned int if_idx, char if_name[32])
{
    struct if_rx_tx_bytes *entry = (struct if_rx_tx_bytes *)malloc(sizeof(struct if_rx_tx_bytes));

    memset(entry, 0, sizeof(struct if_rx_tx_bytes));

    entry->if_idx = if_idx;
    strncpy(entry->if_name, if_name, 32);

    entry->timer_interval_sec = TIMER_TICK_SEC;
    
    return entry;
}

static int insert_if_rx_tx_entry(struct if_rx_tx_bytes *entry)
{
    int iif = 0;
    for(iif=0;iif<MAX_IF_NR;iif++) {
        if(if_realtime_speed_entry[iif] == NULL ){
            if_realtime_speed_entry[iif] = entry;
//            printf("%d:%d:%s\n", iif, entry->if_idx, entry->if_name, iif);
            break;
        }
    }
}

static void get_if_realtime_bytes(char *if_name, unsigned long *rx, unsigned long *tx)
{
#define RX_TX_CMD "cat /proc/net/dev | grep %s | sed 's/:/ /g' | awk '{print $2\" \" $10}'"   

    char line[512] = {0};
    char cmd[512] = {0};

    snprintf(cmd, 512, RX_TX_CMD, if_name);
    
    FILE *fp = popen(cmd, "r");
    
    fgets(line, 512, fp);
    sscanf(line, "%ld %ld", rx, tx);
//    printf("%s >> %s >> %ld %ld\n", cmd, line, *rx, *tx);
    pclose(fp);
}



static int cal_if_realtime_Bps(struct if_rx_tx_bytes *entry)
{
    unsigned long rx_bytes = entry->rx_bytes_next - entry->rx_bytes_prev;
    unsigned long tx_bytes = entry->tx_bytes_next - entry->tx_bytes_prev;

    entry->rx_Bps = rx_bytes / TIMER_TICK_SEC;
    entry->tx_Bps = tx_bytes / TIMER_TICK_SEC;
    
//    printf(">>>> %ld(%ld)  %ld(%ld)\n", entry->rx_Bps, rx_bytes, entry->tx_Bps, tx_bytes);
}


/* 创建Socket */
static int if_socket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        return -1;
    }
    return sock;
}

/* 获取所有网口的句柄信息 */
static int get_ifconf_r(int sockfd, struct ifconf *ifc, void *buf, int buf_len)
{
    if(sockfd <= 2 || !ifc || !buf || buf_len <= 0) {
        return -1;
    }

    ifc->ifc_len = buf_len;
    ifc->ifc_buf = buf;
    if (ioctl(sockfd, SIOCGIFCONF, ifc) == -1) {
        return -1;
    }
    return 0;
}

/* 获取网口状态 */
static int get_ifstat(int sockfd, const char *if_name, int *if_flag)
{
    if(sockfd <= 2 || !if_name || !if_flag) {
        return -1;
    }
    struct ifreq ifr;
    *if_flag = 0;
        
    strcpy(ifr.ifr_name, if_name);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == 0) {
        if(ifr.ifr_flags & IFF_UP) *if_flag |= IFSTAT_UP;
        if(ifr.ifr_flags & IFF_BROADCAST) *if_flag |= IFSTAT_BROADCAST;
        if(ifr.ifr_flags & IFF_LOOPBACK) *if_flag |= IFSTAT_LOOPBACK;
        if(ifr.ifr_flags & IFF_POINTOPOINT) *if_flag |= IFSTAT_POINTOPOINT;
        if(ifr.ifr_flags & IFF_MULTICAST) *if_flag |= IFSTAT_MULTICAST;
    }else{
        return -1;
    }
    return 0;
}

/* 查询网口 IPv4 地址 */
static int get_ifaddr(int sockfd, const char *if_name, char *addr, int addr_len)
{
    if(sockfd <= 2 || !if_name || !addr || addr_len <= 0) {
        return -1;
    }
    struct ifreq ifr;
    
    strcpy(ifr.ifr_name, if_name);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == 0) {
        if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
            snprintf(addr,addr_len,"%s",
                        inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
        }
    }else{
        return -1;
    }
    return 0;
}

/* 查询网口 MAC 地址 */
static int get_ifhwaddr(int sockfd, const char *if_name, char *hwaddr, int hwaddr_len)
{
    if(sockfd <= 2 || !if_name || !hwaddr || hwaddr_len <= 0) {
        return -1;
    }
    struct ifreq ifr;
    
    strcpy(ifr.ifr_name, if_name);
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
        unsigned char * ptr ;
        ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        snprintf(hwaddr,hwaddr_len,"%02X:%02X:%02X:%02X:%02X:%02X",
                        *ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
    }else{
        return -1;
    }
    return 0;
}

/* 查询网口带宽 */
static int get_ifethspeed(int sockfd, const char *if_name, unsigned int *speed)
{
    struct ifreq ifr;
    struct ethtool_cmd ep;
    
    strcpy(ifr.ifr_name, if_name);
    
    ep.cmd = ETHTOOL_GSET; 
    ifr.ifr_data = (caddr_t)&ep;  

    if (ioctl(sockfd, SIOCETHTOOL, &ifr) != 0) { // 如果出错退出; 
        return -1;
    }
    *speed = ep.speed;
    return 0;
}

static void * get_realtime_speed(void*arg)
{
    int iif = 0;
    while(1) {
        for(iif=0;iif<MAX_IF_NR;iif++) {
            if(if_realtime_speed_entry[iif] != NULL ){
                struct if_rx_tx_bytes *entry = if_realtime_speed_entry[iif];
                get_if_realtime_bytes(entry->if_name, &entry->rx_bytes_prev, &entry->tx_bytes_prev);
                sleep(TIMER_TICK_SEC);
                get_if_realtime_bytes(entry->if_name, &entry->rx_bytes_next, &entry->tx_bytes_next);
                
                pthread_rwlock_wrlock(&realtime_speed_lock);
                cal_if_realtime_Bps(entry);
                pthread_rwlock_unlock(&realtime_speed_lock);
            }
        }
//        sleep(TIMER_TICK_SEC);
    }
}

static void get_if_realtime_speed(struct ifinfo *info)
{
    int iif = 0;
    for(iif=0;iif<MAX_IF_NR;iif++) {
        if(if_realtime_speed_entry[iif] != NULL ){
            struct if_rx_tx_bytes *entry = if_realtime_speed_entry[iif];
            if(strcmp(info->if_name, entry->if_name) == 0) {
                pthread_rwlock_rdlock(&realtime_speed_lock);
                info->if_realtime_speed.rx_Bps = entry->rx_Bps;
                info->if_realtime_speed.tx_Bps = entry->tx_Bps;
                pthread_rwlock_unlock(&realtime_speed_lock);
            }
        }
    }
}

static  __attribute__((constructor(101))) void __try_to_get_realtime_speed_start() 
{
    struct ifconf ifc;
    
    char buf[2048];
    int ret = 0;    
    int count = 0;
    int sock = if_socket();/* 创建socket */
    
    ret = get_ifconf_r(sock, &ifc, buf, sizeof(buf));
    if(ret != 0) {
        close(sock);
        return ;
    }
    /* 巧妙的网口结构保存 */
    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    /* 轮询所有网口 */
    for (; it != end; ++it, count++) {
        struct if_rx_tx_bytes *entry = new_if_rx_tx_entry(if_nametoindex(it->ifr_name), it->ifr_name);
        insert_if_rx_tx_entry(entry);
    }
    close(sock);
#if 0 //创建定时器查询网口试试速率
#else //创建线程
    pthread_create(&tid_get_realtime_speed, NULL, get_realtime_speed, NULL);
#endif
}


/* 查询网口实时速率 */
static int get_ifrealspeed(const char *if_name, unsigned long *Bps)
{
    
}

/**
 *  get_ifinfo - 获取网口信息
 *
 *    该接口可以获取网口的速率，但是，在虚拟机环境中，无法查询网口带宽
 *  
 *  display_fn  查询的回调函数，参见： ifinfo_display 说明
 *  arg         回调函数被调用时的 arg 参数，该参数可以为 NULL
 *  
 *  return      失败，返回 -1， 成功，返回查询的网口总数
 */
int get_ifinfo(ifinfo_display display_fn, void *arg)
{
    /* 如果回调函数为空，则返回失败 */
    if(!display_fn) {
        return -1;
    }

    struct ifconf ifc;
    struct ifinfo info;
    
    char buf[2048];
    int ret = 0;    
    int count = 0;

    int sock = if_socket();/* 创建socket */
    
    ret = get_ifconf_r(sock, &ifc, buf, sizeof(buf));
    if(ret != 0) {
        close(sock);
        return -1;
    }

    /* 巧妙的网口结构保存 */
    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    /* 轮询所有网口 */
    for (; it != end; ++it, count++) {
        memset(&info, 0, sizeof(struct ifinfo));
        
        strcpy(info.if_name, it->ifr_name);
        
        info.if_idx = if_nametoindex(info.if_name);

        /* 获取一系列的网口信息，当然可以在后面继续开发，
            但不要忘记在 struct ifinfo 添加对应的单数字段 */
        get_ifstat(sock, info.if_name, &info.if_flag);
        get_ifaddr(sock, info.if_name, info.if_ipv4, sizeof(info.if_ipv4));
        get_ifhwaddr(sock, info.if_name, info.if_eth.if_ethmac, sizeof(info.if_eth.if_ethmac));
        get_ifethspeed(sock, info.if_name, &info.if_eth.if_ethspeed);

        get_if_realtime_speed(&info);
        
        /* 在这里，回调函数将被调用 */
        if(display_fn) display_fn(&info, arg);
    }

    close(sock);

    return count;
}


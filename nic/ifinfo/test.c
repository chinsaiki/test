
#include <stdio.h>

#include "ifinfo.h"

//function print_speed()
//{
//	bytes=$1
//	if [[ $bytes -lt 1024 ]]; then
//		print_bytes="$bytes B/s"
//	elif [[ $bytes -gt 1073741824 ]]; then
//		print_bytes=$(echo $bytes | awk '{print $1/1073741824 " GB/s"}')
//	elif [[ $bytes -gt 1048576 ]]; then
//		print_bytes=$(echo $bytes | awk '{print $1/1048576 " MB/s"}')
//	else
//		print_bytes=$(echo $bytes | awk '{print $1/1024 " KB/s"}')
//	fi
//	echo $print_bytes
//}



void ifdisplay(const struct ifinfo *info, void *arg)
{
    printf("%2d: %-10s %16s %18s %d Mb  ",
            info->if_idx, info->if_name,info->if_ipv4, info->if_eth.if_ethmac,info->if_eth.if_ethspeed);

    if (info->if_realtime_speed.rx_Bps < 1024) {
        printf("RX %ld B/s ", info->if_realtime_speed.rx_Bps);
    } else if (info->if_realtime_speed.rx_Bps > 1073741824) {
        printf("RX %ld GB/s ", info->if_realtime_speed.rx_Bps/1073741824);

    } else if (info->if_realtime_speed.rx_Bps > 1048576) {
        printf("RX %ld MB/s ", info->if_realtime_speed.rx_Bps/1048576);

    } else {
        printf("RX %ld KB/s ", info->if_realtime_speed.rx_Bps/1024);

    }
    if (info->if_realtime_speed.tx_Bps < 1024) {
        printf("TX %ld B/s ", info->if_realtime_speed.tx_Bps);
    } else if (info->if_realtime_speed.tx_Bps > 1073741824) {
        printf("TX %ld GB/s ", info->if_realtime_speed.tx_Bps/1073741824);

    } else if (info->if_realtime_speed.tx_Bps > 1048576) {
        printf("TX %ld MB/s ", info->if_realtime_speed.tx_Bps/1048576);

    } else {
        printf("TX %ld KB/s ", info->if_realtime_speed.tx_Bps/1024);

    }
    printf("\n");
            
}


int main()
{
    int ret = 0;
    while(1) {
        ret = get_ifinfo(ifdisplay, NULL);
        printf("Total %d interface.\n", ret);
        sleep(1);
    }
}


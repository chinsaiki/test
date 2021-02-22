ip --force link set dev enp0s8 xdp obj bpf_filter_kern.o sec xdp_action

xtables-monitor -e |xargs -i ./bpf_filter {} /sys/fs/bpf/xdp/globals/action_map

iptables-nft -A INPUT -s 192.168.56.110 -j DROP

...

iptables-nft -D INPUT -s 192.168.56.110 -j DROP


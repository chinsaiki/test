// bpf_filter_kern.c
#include <uapi/linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <uapi/linux/bpf.h>
#include "bpf_helpers.h"

#define PIN_GLOBAL_NS		2
struct bpf_elf_map {
	__u32 type;
	__u32 size_key;
	__u32 size_value;
	__u32 max_elem;
	__u32 flags;
	__u32 id;
	__u32 pinning;
};

// 系统范围内的全局map
struct bpf_elf_map SEC("maps") action_map = {
	.type = BPF_MAP_TYPE_HASH,
	.size_key = sizeof(int),
	.size_value = sizeof(int),
	.pinning        = PIN_GLOBAL_NS,
	.max_elem = 100,
};

static inline int parse_ipv4(void *data, u64 nh_off, void *data_end,
			     __be32 *src, __be32 *dest)
{
	struct iphdr *iph = data + nh_off;

	if (iph + 1 > data_end)
		return 0;
	*src = iph->saddr;
	*dest = iph->daddr;
	return iph->protocol;
}

// 默认策略为ACCEPT的处理逻辑本身
SEC("xdp_action") // 注意iproute2的section字段
int xdp_drop_prog(struct xdp_md *ctx)
{
	void *data_end = (void *)(long)ctx->data_end;
	void *data = (void *)(long)ctx->data;
	int *action_entry = NULL;
	int in_index = ctx->ingress_ifindex, *out_index;
	__be32 src_ip = 0, dest_ip = 0;
	struct ethhdr *eth = (struct ethhdr *)data;
	u16 h_proto;
	u64 nh_off;
	u32 ipproto;

	nh_off = sizeof(*eth);
	if (data + nh_off > data_end) {
		return XDP_DROP;
	}

	h_proto = eth->h_proto;
	if (h_proto != htons(ETH_P_IP))
		return XDP_PASS;
	ipproto = parse_ipv4(data, nh_off, data_end, &src_ip, &dest_ip);
	action_entry = bpf_map_lookup_elem(&action_map, &src_ip);
	if (action_entry) {
		if (*action_entry == 0)
			return XDP_PASS;
		else if (*action_entry == 1)
			return XDP_DROP;
	}
	// Default policy PASS
	return  XDP_PASS;
}

char _license[] SEC("license") = "GPL";


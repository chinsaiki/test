
enum gro_result {
	GRO_MERGED,
	GRO_MERGED_FREE,
	GRO_HELD,
	GRO_NORMAL,
	GRO_DROP,
	GRO_CONSUMED,
};
typedef enum gro_result gro_result_t;



gro_result_t napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb)
{
	gro_result_t ret;

	skb_mark_napi_id(skb, napi);
	trace_napi_gro_receive_entry(skb);

	skb_gro_reset_offset(skb);

	ret = napi_skb_finish(dev_gro_receive(napi, skb), skb);
	trace_napi_gro_receive_exit(ret);

	return ret;
}
EXPORT_SYMBOL(napi_gro_receive);

/* used in the NIC receive handler to mark the skb */
static inline void skb_mark_napi_id(struct sk_buff *skb,
				    struct napi_struct *napi)
{
#ifdef CONFIG_NET_RX_BUSY_POLL
	skb->napi_id = napi->napi_id;
#endif
}
                    
static void skb_gro_reset_offset(struct sk_buff *skb)
{
    const struct skb_shared_info *pinfo = skb_shinfo(skb);
    const skb_frag_t *frag0 = &pinfo->frags[0];

    NAPI_GRO_CB(skb)->data_offset = 0;
    NAPI_GRO_CB(skb)->frag0 = NULL;
    NAPI_GRO_CB(skb)->frag0_len = 0;

    if (skb_mac_header(skb) == skb_tail_pointer(skb) &&
        pinfo->nr_frags &&
        !PageHighMem(skb_frag_page(frag0))) {
        NAPI_GRO_CB(skb)->frag0 = skb_frag_address(frag0);
        NAPI_GRO_CB(skb)->frag0_len = min_t(unsigned int,
                            skb_frag_size(frag0),
                            skb->end - skb->tail);
    }
}

static void napi_skb_free_stolen_head(struct sk_buff *skb)
{
	skb_dst_drop(skb);
	secpath_reset(skb);
	kmem_cache_free(skbuff_head_cache, skb);
}

static gro_result_t napi_skb_finish(gro_result_t ret, struct sk_buff *skb)
{
	switch (ret) {
	case GRO_NORMAL:
		if (netif_receive_skb_internal(skb))
			ret = GRO_DROP;
		break;

	case GRO_DROP:
		kfree_skb(skb);
		break;

	case GRO_MERGED_FREE:
		if (NAPI_GRO_CB(skb)->free == NAPI_GRO_FREE_STOLEN_HEAD)
			napi_skb_free_stolen_head(skb);
		else
			__kfree_skb(skb);
		break;

	case GRO_HELD:
	case GRO_MERGED:
	case GRO_CONSUMED:
		break;
	}

	return ret;
}

static enum gro_result dev_gro_receive(struct napi_struct *napi, struct sk_buff *skb)
{
	u32 hash = skb_get_hash_raw(skb) & (GRO_HASH_BUCKETS - 1);
	struct list_head *head = &offload_base;
	struct packet_offload *ptype;
	__be16 type = skb->protocol;
	struct list_head *gro_head;
	struct sk_buff *pp = NULL;
	enum gro_result ret;
	int same_flow;
	int grow;

	if (netif_elide_gro(skb->dev))
		goto normal;

	gro_head = gro_list_prepare(napi, skb);

	rcu_read_lock();
	list_for_each_entry_rcu(ptype, head, list) {
		if (ptype->type != type || !ptype->callbacks.gro_receive)
			continue;

		skb_set_network_header(skb, skb_gro_offset(skb));
		skb_reset_mac_len(skb);
		NAPI_GRO_CB(skb)->same_flow = 0;
		NAPI_GRO_CB(skb)->flush = skb_is_gso(skb) || skb_has_frag_list(skb);
		NAPI_GRO_CB(skb)->free = 0;
		NAPI_GRO_CB(skb)->encap_mark = 0;
		NAPI_GRO_CB(skb)->recursion_counter = 0;
		NAPI_GRO_CB(skb)->is_fou = 0;
		NAPI_GRO_CB(skb)->is_atomic = 1;
		NAPI_GRO_CB(skb)->gro_remcsum_start = 0;

		/* Setup for GRO checksum validation */
		switch (skb->ip_summed) {
		case CHECKSUM_COMPLETE:
			NAPI_GRO_CB(skb)->csum = skb->csum;
			NAPI_GRO_CB(skb)->csum_valid = 1;
			NAPI_GRO_CB(skb)->csum_cnt = 0;
			break;
		case CHECKSUM_UNNECESSARY:
			NAPI_GRO_CB(skb)->csum_cnt = skb->csum_level + 1;
			NAPI_GRO_CB(skb)->csum_valid = 0;
			break;
		default:
			NAPI_GRO_CB(skb)->csum_cnt = 0;
			NAPI_GRO_CB(skb)->csum_valid = 0;
		}

		pp = INDIRECT_CALL_INET(ptype->callbacks.gro_receive,
					ipv6_gro_receive, inet_gro_receive,
					gro_head, skb);
		break;
	}
	rcu_read_unlock();

	if (&ptype->list == head)
		goto normal;

	if (IS_ERR(pp) && PTR_ERR(pp) == -EINPROGRESS) {
		ret = GRO_CONSUMED;
		goto ok;
	}

	same_flow = NAPI_GRO_CB(skb)->same_flow;
	ret = NAPI_GRO_CB(skb)->free ? GRO_MERGED_FREE : GRO_MERGED;

	if (pp) {
		skb_list_del_init(pp);
		napi_gro_complete(pp);
		napi->gro_hash[hash].count--;
	}

	if (same_flow)
		goto ok;

	if (NAPI_GRO_CB(skb)->flush)
		goto normal;

	if (unlikely(napi->gro_hash[hash].count >= MAX_GRO_SKBS)) {
		gro_flush_oldest(gro_head);
	} else {
		napi->gro_hash[hash].count++;
	}
	NAPI_GRO_CB(skb)->count = 1;
	NAPI_GRO_CB(skb)->age = jiffies;
	NAPI_GRO_CB(skb)->last = skb;
	skb_shinfo(skb)->gso_size = skb_gro_len(skb);
	list_add(&skb->list, gro_head);
	ret = GRO_HELD;

pull:
	grow = skb_gro_offset(skb) - skb_headlen(skb);
	if (grow > 0)
		gro_pull_from_frag0(skb, grow);
ok:
	if (napi->gro_hash[hash].count) {
		if (!test_bit(hash, &napi->gro_bitmask))
			__set_bit(hash, &napi->gro_bitmask);
	} else if (test_bit(hash, &napi->gro_bitmask)) {
		__clear_bit(hash, &napi->gro_bitmask);
	}

	return ret;

normal:
	ret = GRO_NORMAL;
	goto pull;
}


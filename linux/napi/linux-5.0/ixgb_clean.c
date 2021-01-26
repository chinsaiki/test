
/* board specific private data structure */

struct ixgb_adapter {
	struct timer_list watchdog_timer;
	unsigned long active_vlans[BITS_TO_LONGS(VLAN_N_VID)];
	u32 bd_number;
	u32 rx_buffer_len;
	u32 part_num;
	u16 link_speed;
	u16 link_duplex;
	struct work_struct tx_timeout_task;

	/* TX */
	struct ixgb_desc_ring tx_ring ____cacheline_aligned_in_smp;
	unsigned int restart_queue;
	unsigned long timeo_start;
	u32 tx_cmd_type;
	u64 hw_csum_tx_good;
	u64 hw_csum_tx_error;
	u32 tx_int_delay;
	u32 tx_timeout_count;
	bool tx_int_delay_enable;
	bool detect_tx_hung;

	/* RX */
	struct ixgb_desc_ring rx_ring;
	u64 hw_csum_rx_error;
	u64 hw_csum_rx_good;
	u32 rx_int_delay;
	bool rx_csum;

	/* OS defined structs */
	struct napi_struct napi;
	struct net_device *netdev;
	struct pci_dev *pdev;

	/* structs defined in ixgb_hw.h */
	struct ixgb_hw hw;
	u16 msg_enable;
	struct ixgb_hw_stats stats;
	u32 alloc_rx_buff_failed;
	bool have_msi;
	unsigned long flags;
};

enum ixgb_state_t {
	/* TBD
	__IXGB_TESTING,
	__IXGB_RESETTING,
	*/
	__IXGB_DOWN
};

/**
 * ixgb_clean - NAPI Rx polling callback
 * @adapter: board private structure
 **/

static int
ixgb_clean(struct napi_struct *napi, int budget)
{
	struct ixgb_adapter *adapter = container_of(napi, struct ixgb_adapter, napi);
	int work_done = 0;

	ixgb_clean_tx_irq(adapter);
	ixgb_clean_rx_irq(adapter, &work_done, budget);

	/* If budget not fully consumed, exit the polling mode */
	if (work_done < budget) {
		napi_complete_done(napi, work_done);
		if (!test_bit(__IXGB_DOWN, &adapter->flags))
			ixgb_irq_enable(adapter);
	}

	return work_done;
}

/**
 * ixgb_clean_tx_irq - Reclaim resources after transmit completes
 * @adapter: board private structure
 **/

static bool
ixgb_clean_tx_irq(struct ixgb_adapter *adapter)
{
	struct ixgb_desc_ring *tx_ring = &adapter->tx_ring;
	struct net_device *netdev = adapter->netdev;
	struct ixgb_tx_desc *tx_desc, *eop_desc;
	struct ixgb_buffer *buffer_info;
	unsigned int i, eop;
	bool cleaned = false;

	i = tx_ring->next_to_clean;
	eop = tx_ring->buffer_info[i].next_to_watch;
	eop_desc = IXGB_TX_DESC(*tx_ring, eop);

	while (eop_desc->status & IXGB_TX_DESC_STATUS_DD) {

		rmb(); /* read buffer_info after eop_desc */
		for (cleaned = false; !cleaned; ) {
			tx_desc = IXGB_TX_DESC(*tx_ring, i);
			buffer_info = &tx_ring->buffer_info[i];

			if (tx_desc->popts &
			   (IXGB_TX_DESC_POPTS_TXSM |
			    IXGB_TX_DESC_POPTS_IXSM))
				adapter->hw_csum_tx_good++;

			ixgb_unmap_and_free_tx_resource(adapter, buffer_info);

			*(u32 *)&(tx_desc->status) = 0;

			cleaned = (i == eop);
			if (++i == tx_ring->count) i = 0;
		}

		eop = tx_ring->buffer_info[i].next_to_watch;
		eop_desc = IXGB_TX_DESC(*tx_ring, eop);
	}

	tx_ring->next_to_clean = i;

	if (unlikely(cleaned && netif_carrier_ok(netdev) &&
		     IXGB_DESC_UNUSED(tx_ring) >= DESC_NEEDED)) {
		/* Make sure that anybody stopping the queue after this
		 * sees the new next_to_clean. */
		smp_mb();

		if (netif_queue_stopped(netdev) &&
		    !(test_bit(__IXGB_DOWN, &adapter->flags))) {
			netif_wake_queue(netdev);
			++adapter->restart_queue;
		}
	}

	if (adapter->detect_tx_hung) {
		/* detect a transmit hang in hardware, this serializes the
		 * check with the clearing of time_stamp and movement of i */
		adapter->detect_tx_hung = false;
		if (tx_ring->buffer_info[eop].time_stamp &&
		   time_after(jiffies, tx_ring->buffer_info[eop].time_stamp + HZ)
		   && !(IXGB_READ_REG(&adapter->hw, STATUS) &
		        IXGB_STATUS_TXOFF)) {
			/* detected Tx unit hang */
			netif_err(adapter, drv, adapter->netdev,
				  "Detected Tx Unit Hang\n"
				  "  TDH                  <%x>\n"
				  "  TDT                  <%x>\n"
				  "  next_to_use          <%x>\n"
				  "  next_to_clean        <%x>\n"
				  "buffer_info[next_to_clean]\n"
				  "  time_stamp           <%lx>\n"
				  "  next_to_watch        <%x>\n"
				  "  jiffies              <%lx>\n"
				  "  next_to_watch.status <%x>\n",
				  IXGB_READ_REG(&adapter->hw, TDH),
				  IXGB_READ_REG(&adapter->hw, TDT),
				  tx_ring->next_to_use,
				  tx_ring->next_to_clean,
				  tx_ring->buffer_info[eop].time_stamp,
				  eop,
				  jiffies,
				  eop_desc->status);
			netif_stop_queue(netdev);
		}
	}

	return cleaned;
}

/**
 * ixgb_clean_rx_irq - Send received data up the network stack,
 * @adapter: board private structure
 **/

static bool
ixgb_clean_rx_irq(struct ixgb_adapter *adapter, int *work_done, int work_to_do)
{
	struct ixgb_desc_ring *rx_ring = &adapter->rx_ring;
	struct net_device *netdev = adapter->netdev;
	struct pci_dev *pdev = adapter->pdev;
	struct ixgb_rx_desc *rx_desc, *next_rxd;
	struct ixgb_buffer *buffer_info, *next_buffer, *next2_buffer;
	u32 length;
	unsigned int i, j;
	int cleaned_count = 0;
	bool cleaned = false;

	i = rx_ring->next_to_clean;
	rx_desc = IXGB_RX_DESC(*rx_ring, i);
	buffer_info = &rx_ring->buffer_info[i];

	while (rx_desc->status & IXGB_RX_DESC_STATUS_DD) {
		struct sk_buff *skb;
		u8 status;

		if (*work_done >= work_to_do)
			break;

		(*work_done)++;
		rmb();	/* read descriptor and rx_buffer_info after status DD */
		status = rx_desc->status;
		skb = buffer_info->skb;
		buffer_info->skb = NULL;

		prefetch(skb->data - NET_IP_ALIGN);

		if (++i == rx_ring->count)
			i = 0;
		next_rxd = IXGB_RX_DESC(*rx_ring, i);
		prefetch(next_rxd);

		j = i + 1;
		if (j == rx_ring->count)
			j = 0;
		next2_buffer = &rx_ring->buffer_info[j];
		prefetch(next2_buffer);

		next_buffer = &rx_ring->buffer_info[i];

		cleaned = true;
		cleaned_count++;

		dma_unmap_single(&pdev->dev,
				 buffer_info->dma,
				 buffer_info->length,
				 DMA_FROM_DEVICE);
		buffer_info->dma = 0;

		length = le16_to_cpu(rx_desc->length);
		rx_desc->length = 0;

		if (unlikely(!(status & IXGB_RX_DESC_STATUS_EOP))) {

			/* All receives must fit into a single buffer */

			pr_debug("Receive packet consumed multiple buffers length<%x>\n",
				 length);

			dev_kfree_skb_irq(skb);
			goto rxdesc_done;
		}

		if (unlikely(rx_desc->errors &
		    (IXGB_RX_DESC_ERRORS_CE | IXGB_RX_DESC_ERRORS_SE |
		     IXGB_RX_DESC_ERRORS_P | IXGB_RX_DESC_ERRORS_RXE))) {
			dev_kfree_skb_irq(skb);
			goto rxdesc_done;
		}

		ixgb_check_copybreak(&adapter->napi, buffer_info, length, &skb);

		/* Good Receive */
		skb_put(skb, length);

		/* Receive Checksum Offload */
		ixgb_rx_checksum(adapter, rx_desc, skb);

		skb->protocol = eth_type_trans(skb, netdev);
		if (status & IXGB_RX_DESC_STATUS_VP)
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q),
				       le16_to_cpu(rx_desc->special));

		netif_receive_skb(skb);

rxdesc_done:
		/* clean up descriptor, might be written over by hw */
		rx_desc->status = 0;

		/* return some buffers to hardware, one at a time is too slow */
		if (unlikely(cleaned_count >= IXGB_RX_BUFFER_WRITE)) {
			ixgb_alloc_rx_buffers(adapter, cleaned_count);
			cleaned_count = 0;
		}

		/* use prefetched values */
		rx_desc = next_rxd;
		buffer_info = next_buffer;
	}

	rx_ring->next_to_clean = i;

	cleaned_count = IXGB_DESC_UNUSED(rx_ring);
	if (cleaned_count)
		ixgb_alloc_rx_buffers(adapter, cleaned_count);

	return cleaned;
}


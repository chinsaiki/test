整体结构


		non-NAPI				NAPI


					IRQ
					 |
			+--------+-----------+
			|					 |
		netif_rx			napi_schedule
			|					 |
			|					 |
   enqueue_to_backlog		__napi_schedule				top half
			|					 |
			|					 |
			|				____napi_schedule
			|					 |
			|					 |
	add skb to \			napi_struct add to \
	input_kpt_queue \		poll_list
	backlog add to \			 |
	poll_list					 |
			|			__raise_softirq_irqoff(NET_RX_SOFTIRQ);
			|					 |
			+--------+-----------+
					 |
				  softirq
					 |
				net_rx_action
					 |
			+--------+-----------+
			|					 |
			|					 |
	process_backlog			driver POLL function
			|					 |
			|					 |
			|				napi_gro_receive			bottom half
			|					 |
			|					 |
			|				netif_receive_skb
			|					 |
			+--------+-----------+
					 |
					 |
				__netif_receive_skb









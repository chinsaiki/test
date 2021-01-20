# This is a configure file for the trace-c.sh
#
export FLAGS
FLAGS=${FLAGS}" -k"
FLAGS=${FLAGS}" -p function"
FLAGS=${FLAGS}" -b 65534"	# default buffer size per CPU
#VFS
FLAGS=${FLAGS}" -e sys_enter_write"	#syscall
FLAGS=${FLAGS}" -e sys_enter_read"	#syscall
FLAGS=${FLAGS}" -e sys_exit_write"	#syscall
FLAGS=${FLAGS}" -e sys_exit_read"	#syscall
#EXT4
FLAGS=${FLAGS}" -e ext4_sync_file_enter"
FLAGS=${FLAGS}" -e ext4_direct_IO_enter"
FLAGS=${FLAGS}" -e ext4_direct_IO_exit"
FLAGS=${FLAGS}" -e ext4_sync_file_exit"
#FLAGS=${FLAGS}" -e ext4_da_write_begin"
#FLAGS=${FLAGS}" -e ext4_da_write_end"
FLAGS=${FLAGS}" -e ext4_writepages"
FLAGS=${FLAGS}" -e ext4_writepages_result"
#FLAGS=${FLAGS}" -e ext4_es_lookup_extent_enter"
#FLAGS=${FLAGS}" -e ext4_es_lookup_extent_exit"
FLAGS=${FLAGS}" -l ext4_file_write_iter"
FLAGS=${FLAGS}" -l generic_file_read_iter"
FLAGS=${FLAGS}" -l __generic_file_write_iter"
FLAGS=${FLAGS}" -l generic_perform_write"
#Block
FLAGS=${FLAGS}" -e block_bio_remap"
FLAGS=${FLAGS}" -e block_bio_queue"
FLAGS=${FLAGS}" -e block_getrq"
FLAGS=${FLAGS}" -e block_plug"
FLAGS=${FLAGS}" -e block_unplug"
FLAGS=${FLAGS}" -e block_rq_issue"
FLAGS=${FLAGS}" -e block_rq_insert"
FLAGS=${FLAGS}" -e block_rq_complete"
FLAGS=${FLAGS}" -l blk_start_plug"
#SCSI
FLAGS=${FLAGS}" -e scsi_dispatch_cmd_start"
FLAGS=${FLAGS}" -e scsi_dispatch_cmd_done"
FLAGS=${FLAGS}" -l scsi_dma_map"
FLAGS=${FLAGS}" -l scsi_queue_rq"
#HW IRQ
FLAGS=${FLAGS}" -e irq_handler_entry"
FLAGS=${FLAGS}" -l ufshcd_intr"		#UFS
FLAGS=${FLAGS}" -l scsi_queue_rq"
FLAGS=${FLAGS}" -l scsi_dma_map"
FLAGS=${FLAGS}" -l scsi_dma_unmap"
#SW IRQ 
FLAGS=${FLAGS}" -l scsi_softirq_done"
#FLAGS=${FLAGS}" -e ext4"
#FLAGS=${FLAGS}" -e block"


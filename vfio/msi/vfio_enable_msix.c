
/**
 * The interrupt source type, e.g. UIO, VFIO, ALARM etc.
 */
enum rte_intr_handle_type {
	RTE_INTR_HANDLE_UNKNOWN = 0,  /**< generic unknown handle */
	RTE_INTR_HANDLE_UIO,          /**< uio device handle */
	RTE_INTR_HANDLE_UIO_INTX,     /**< uio generic handle */
	RTE_INTR_HANDLE_VFIO_LEGACY,  /**< vfio device handle (legacy) */
	RTE_INTR_HANDLE_VFIO_MSI,     /**< vfio device handle (MSI) */
	RTE_INTR_HANDLE_VFIO_MSIX,    /**< vfio device handle (MSIX) */
	RTE_INTR_HANDLE_ALARM,        /**< alarm handle */
	RTE_INTR_HANDLE_EXT,          /**< external handler */
	RTE_INTR_HANDLE_VDEV,         /**< virtual device */
	RTE_INTR_HANDLE_DEV_EVENT,    /**< device event handle */
	RTE_INTR_HANDLE_VFIO_REQ,     /**< VFIO request handle */
	RTE_INTR_HANDLE_MAX           /**< count of elements */
};

/** interrupt epoll event obj, taken by epoll_event.ptr */
struct rte_epoll_event {
	volatile uint32_t status;  /**< OUT: event status */
	int fd;                    /**< OUT: event fd */
	int epfd;       /**< OUT: epoll instance the ev associated with */
	struct rte_epoll_data epdata;
};

/** Handle for interrupts. */
struct rte_intr_handle {
	RTE_STD_C11
	union {
		int vfio_dev_fd;  /**< VFIO device file descriptor */
		int uio_cfg_fd;  /**< UIO cfg file desc for uio_pci_generic */
	};
	int fd;	 /**< interrupt event file descriptor */
	enum rte_intr_handle_type type;  /**< handle type */
	uint32_t max_intr;             /**< max interrupt requested */
	uint32_t nb_efd;               /**< number of available efd(event fd) */
	uint8_t efd_counter_size;      /**< size of efd counter, used for vdev */
	int efds[RTE_MAX_RXTX_INTR_VEC_ID];  /**< intr vectors/efds mapping */
	struct rte_epoll_event elist[RTE_MAX_RXTX_INTR_VEC_ID];
				       /**< intr vector epoll event */
	int *intr_vec;                 /**< intr vector number array */
};

/* enable MSI interrupts */
static int
vfio_enable_msi(const struct rte_intr_handle *intr_handle) {
	int len, ret;
	char irq_set_buf[IRQ_SET_BUF_LEN];
	struct vfio_irq_set *irq_set;
	int *fd_ptr;

	len = sizeof(irq_set_buf);

	irq_set = (struct vfio_irq_set *) irq_set_buf;
	irq_set->argsz = len;
	irq_set->count = 1;
	irq_set->flags = VFIO_IRQ_SET_DATA_EVENTFD | VFIO_IRQ_SET_ACTION_TRIGGER;
	irq_set->index = VFIO_PCI_MSI_IRQ_INDEX;
	irq_set->start = 0;
	fd_ptr = (int *) &irq_set->data;
	*fd_ptr = intr_handle->fd;

	ret = ioctl(intr_handle->vfio_dev_fd, VFIO_DEVICE_SET_IRQS, irq_set);

	if (ret) {
		RTE_LOG(ERR, EAL, "Error enabling MSI interrupts for fd %d\n",
						intr_handle->fd);
		return -1;
	}
	return 0;
}

/* disable MSI interrupts */
static int
vfio_disable_msi(const struct rte_intr_handle *intr_handle) {
	struct vfio_irq_set *irq_set;
	char irq_set_buf[IRQ_SET_BUF_LEN];
	int len, ret;

	len = sizeof(struct vfio_irq_set);

	irq_set = (struct vfio_irq_set *) irq_set_buf;
	irq_set->argsz = len;
	irq_set->count = 0;
	irq_set->flags = VFIO_IRQ_SET_DATA_NONE | VFIO_IRQ_SET_ACTION_TRIGGER;
	irq_set->index = VFIO_PCI_MSI_IRQ_INDEX;
	irq_set->start = 0;

	ret = ioctl(intr_handle->vfio_dev_fd, VFIO_DEVICE_SET_IRQS, irq_set);

	if (ret)
		RTE_LOG(ERR, EAL,
			"Error disabling MSI interrupts for fd %d\n", intr_handle->fd);

	return ret;
}

/* enable MSI-X interrupts */
static int
vfio_enable_msix(const struct rte_intr_handle *intr_handle) {
	int len, ret;
	char irq_set_buf[MSIX_IRQ_SET_BUF_LEN];
	struct vfio_irq_set *irq_set;
	int *fd_ptr;

	len = sizeof(irq_set_buf);

	irq_set = (struct vfio_irq_set *) irq_set_buf;
	irq_set->argsz = len;
	/* 0 < irq_set->count < RTE_MAX_RXTX_INTR_VEC_ID + 1 */
	irq_set->count = intr_handle->max_intr ?
		(intr_handle->max_intr > RTE_MAX_RXTX_INTR_VEC_ID + 1 ?
		RTE_MAX_RXTX_INTR_VEC_ID + 1 : intr_handle->max_intr) : 1;
	irq_set->flags = VFIO_IRQ_SET_DATA_EVENTFD | VFIO_IRQ_SET_ACTION_TRIGGER;
	irq_set->index = VFIO_PCI_MSIX_IRQ_INDEX;
	irq_set->start = 0;
	fd_ptr = (int *) &irq_set->data;
	/* INTR vector offset 0 reserve for non-efds mapping */
	fd_ptr[RTE_INTR_VEC_ZERO_OFFSET] = intr_handle->fd;
	memcpy(&fd_ptr[RTE_INTR_VEC_RXTX_OFFSET], intr_handle->efds,
		sizeof(*intr_handle->efds) * intr_handle->nb_efd);

	ret = ioctl(intr_handle->vfio_dev_fd, VFIO_DEVICE_SET_IRQS, irq_set);

	if (ret) {
		RTE_LOG(ERR, EAL, "Error enabling MSI-X interrupts for fd %d\n",
						intr_handle->fd);
		return -1;
	}

	return 0;
}

/* disable MSI-X interrupts */
static int
vfio_disable_msix(const struct rte_intr_handle *intr_handle) {
	struct vfio_irq_set *irq_set;
	char irq_set_buf[MSIX_IRQ_SET_BUF_LEN];
	int len, ret;

	len = sizeof(struct vfio_irq_set);

	irq_set = (struct vfio_irq_set *) irq_set_buf;
	irq_set->argsz = len;
	irq_set->count = 0;
	irq_set->flags = VFIO_IRQ_SET_DATA_NONE | VFIO_IRQ_SET_ACTION_TRIGGER;
	irq_set->index = VFIO_PCI_MSIX_IRQ_INDEX;
	irq_set->start = 0;

	ret = ioctl(intr_handle->vfio_dev_fd, VFIO_DEVICE_SET_IRQS, irq_set);

	if (ret)
		RTE_LOG(ERR, EAL,
			"Error disabling MSI-X interrupts for fd %d\n", intr_handle->fd);

	return ret;
}


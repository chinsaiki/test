#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)
#define EAL_UEV_MSG_LEN 4096

#define EAL_UEV_MSG_ELEM_LEN 128

/** Handle for interrupts. */
struct rte_intr_handle {
//	RTE_STD_C11
//	union {
//		int vfio_dev_fd;  /**< VFIO device file descriptor */
//		int uio_cfg_fd;  /**< UIO cfg file desc for uio_pci_generic */
//	};
	int fd;	 /**< interrupt event file descriptor */
//	enum rte_intr_handle_type type;  /**< handle type */
//	uint32_t max_intr;             /**< max interrupt requested */
//	uint32_t nb_efd;               /**< number of available efd(event fd) */
//	uint8_t efd_counter_size;      /**< size of efd counter, used for vdev */
//	int efds[RTE_MAX_RXTX_INTR_VEC_ID];  /**< intr vectors/efds mapping */
//	struct rte_epoll_event elist[RTE_MAX_RXTX_INTR_VEC_ID];
//				       /**< intr vector epoll event */
//	int *intr_vec;                 /**< intr vector number array */
};
/**
 * The device event type.
 */
enum rte_dev_event_type {
	RTE_DEV_EVENT_ADD,	/**< device being added */
	RTE_DEV_EVENT_REMOVE,	/**< device being removed */
	RTE_DEV_EVENT_MAX	/**< max value of this enum */
};

struct rte_dev_event {
	enum rte_dev_event_type type;	/**< device event type */
	int subsystem;			/**< subsystem id */
	char *devname;			/**< device name */
};
/* identify the system layer which reports this event. */
enum eal_dev_event_subsystem {
	EAL_DEV_EVENT_SUBSYSTEM_PCI, /* PCI bus device event */
	EAL_DEV_EVENT_SUBSYSTEM_UIO, /* UIO driver device event */
	EAL_DEV_EVENT_SUBSYSTEM_VFIO, /* VFIO driver device event */
	EAL_DEV_EVENT_SUBSYSTEM_MAX
};

static struct rte_intr_handle intr_handle = {.fd = -1 };
/**
 * @internal
 * DPDK-specific version of strlcpy for systems without
 * libc or libbsd copies of the function
 */
static inline size_t
strlcpy(char *dst, const char *src, size_t size)
{
	return (size_t)snprintf(dst, size, "%s", src);
}

static int
dev_uev_parse(const char *buf, struct rte_dev_event *event, int length)
{
	char action[EAL_UEV_MSG_ELEM_LEN];
	char subsystem[EAL_UEV_MSG_ELEM_LEN];
	char pci_slot_name[EAL_UEV_MSG_ELEM_LEN];
	int i = 0;

	memset(action, 0, EAL_UEV_MSG_ELEM_LEN);
	memset(subsystem, 0, EAL_UEV_MSG_ELEM_LEN);
	memset(pci_slot_name, 0, EAL_UEV_MSG_ELEM_LEN);

	while (i < length) {
		for (; i < length; i++) {
			if (*buf)
				break;
			buf++;
		}
		/**
		 * check device uevent from kernel side, no need to check
		 * uevent from udev.
		 */
		if (!strncmp(buf, "libudev", 7)) {
			buf += 7;
			i += 7;
			return -1;
		}
		if (!strncmp(buf, "ACTION=", 7)) {
			buf += 7;
			i += 7;
			strlcpy(action, buf, sizeof(action));
		} else if (!strncmp(buf, "SUBSYSTEM=", 10)) {
			buf += 10;
			i += 10;
			strlcpy(subsystem, buf, sizeof(subsystem));
		} else if (!strncmp(buf, "PCI_SLOT_NAME=", 14)) {
			buf += 14;
			i += 14;
			strlcpy(pci_slot_name, buf, sizeof(subsystem));
			event->devname = strdup(pci_slot_name);
		}
		for (; i < length; i++) {
			if (*buf == '\0')
				break;
			buf++;
		}
	}

	/* parse the subsystem layer */
	if (!strncmp(subsystem, "uio", 3))
		event->subsystem = EAL_DEV_EVENT_SUBSYSTEM_UIO;
	else if (!strncmp(subsystem, "pci", 3))
		event->subsystem = EAL_DEV_EVENT_SUBSYSTEM_PCI;
	else if (!strncmp(subsystem, "vfio", 4))
		event->subsystem = EAL_DEV_EVENT_SUBSYSTEM_VFIO;
	else
		return -1;

	/* parse the action type */
	if (!strncmp(action, "add", 3))
		event->type = RTE_DEV_EVENT_ADD;
	else if (!strncmp(action, "remove", 6))
		event->type = RTE_DEV_EVENT_REMOVE;
	else
		return -1;
	return 0;
}

static int
dev_uev_socket_fd_create(void)
{
	struct sockaddr_nl addr;
	int ret;
//    int type = SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK;
    int type = SOCK_RAW | SOCK_CLOEXEC;
    
	intr_handle.fd = socket(PF_NETLINK, type,
			                NETLINK_KOBJECT_UEVENT);
	if (intr_handle.fd < 0) {
		RTE_LOG(ERR, EAL, "create uevent fd failed.\n");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = 0;
	addr.nl_groups = 0xffffffff;

	ret = bind(intr_handle.fd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		RTE_LOG(ERR, EAL, "Failed to bind uevent socket.\n");
		goto err;
	}

	return 0;
err:
	close(intr_handle.fd);
	intr_handle.fd = -1;
	return ret;
}

int main()
{
	struct rte_dev_event uevent;
    
	char buf[EAL_UEV_MSG_LEN];
    int ret = dev_uev_socket_fd_create();

    printf("ret = %d, fd = %d\n", ret, intr_handle.fd);

    
	ret = recv(intr_handle.fd, buf, EAL_UEV_MSG_LEN, 0);
	if (ret < 0 && errno == EAGAIN){
		RTE_LOG(DEBUG, EAL, "recv error, ret = %d.\n", ret);
		return;
	}
    
	ret = dev_uev_parse(buf, &uevent, EAL_UEV_MSG_LEN);
	if (ret < 0) {
		RTE_LOG(DEBUG, EAL, "It is not an valid event "
			"that need to be handle.\n");
		return;
	}
    printf("uevent.devname = %s\n", uevent.devname);
    
}

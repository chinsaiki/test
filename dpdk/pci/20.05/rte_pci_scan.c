
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "rte_pci.h"

#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)


/**
 * @internal
 * DPDK-specific version of strlcpy for systems without
 * libc or libbsd copies of the function
 */
static inline size_t
rte_strlcpy(char *dst, const char *src, size_t size)
{
	return (size_t)snprintf(dst, size, "%s", src);
}


#define SYSFS_PCI_DEVICES "/sys/bus/pci/devices"

const char *rte_pci_get_sysfs_path(void)
{
	const char *path = NULL;

	path = getenv("SYSFS_PCI_DEVICES");
	if (path == NULL)
		return SYSFS_PCI_DEVICES;

	return path;
}

/* split string into tokens */
int
rte_strsplit(char *string, int stringlen,
	     char **tokens, int maxtokens, char delim)
{
	int i, tok = 0;
	int tokstart = 1; /* first token is right at start of string */

	if (string == NULL || tokens == NULL)
		goto einval_error;

	for (i = 0; i < stringlen; i++) {
		if (string[i] == '\0' || tok >= maxtokens)
			break;
		if (tokstart) {
			tokstart = 0;
			tokens[tok++] = &string[i];
		}
		if (string[i] == delim) {
			string[i] = '\0';
			tokstart = 1;
		}
	}
	return tok;

einval_error:
	errno = EINVAL;
	return -1;
}

/*
 * split up a pci address into its constituent parts.
 */
static int
parse_pci_addr_format(const char *buf, int bufsize, struct rte_pci_addr *addr)
{
	/* first split on ':' */
	union splitaddr {
		struct {
			char *domain;
			char *bus;
			char *devid;
			char *function;
		};
		char *str[PCI_FMT_NVAL]; /* last element-separator is "." not ":" */
	} splitaddr;

	char *buf_copy = strndup(buf, bufsize);
	if (buf_copy == NULL)
		return -1;

	if (rte_strsplit(buf_copy, bufsize, splitaddr.str, PCI_FMT_NVAL, ':')
			!= PCI_FMT_NVAL - 1)
		goto error;
	/* final split is on '.' between devid and function */
	splitaddr.function = strchr(splitaddr.devid,'.');
	if (splitaddr.function == NULL)
		goto error;
	*splitaddr.function++ = '\0';

	/* now convert to int values */
	errno = 0;
	addr->domain = strtoul(splitaddr.domain, NULL, 16);
	addr->bus = strtoul(splitaddr.bus, NULL, 16);
	addr->devid = strtoul(splitaddr.devid, NULL, 16);
	addr->function = strtoul(splitaddr.function, NULL, 10);
	if (errno != 0)
		goto error;

	free(buf_copy); /* free the copy made with strdup */
	return 0;
error:
	free(buf_copy);
	return -1;
}


bool
rte_pci_ignore_device(const struct rte_pci_addr *pci_addr)
{
//	struct rte_devargs *devargs = pci_devargs_lookup(pci_addr);
//
//	switch (rte_pci_bus.bus.conf.scan_mode) {
//	case RTE_BUS_SCAN_WHITELIST:
//		if (devargs && devargs->policy == RTE_DEV_WHITELISTED)
//			return false;
//		break;
//	case RTE_BUS_SCAN_UNDEFINED:
//	case RTE_BUS_SCAN_BLACKLIST:
//		if (devargs == NULL ||
//		    devargs->policy != RTE_DEV_BLACKLISTED)
//			return false;
//		break;
//	}
	return false;
}

/* parse a sysfs (or other) file containing one integer value */
int
eal_parse_sysfs_value(const char *filename, unsigned long *val)
{
	FILE *f;
	char buf[BUFSIZ];
	char *end = NULL;

	if ((f = fopen(filename, "r")) == NULL) {
		RTE_LOG(ERR, EAL, "%s(): cannot open sysfs value %s\n",
			__func__, filename);
		return -1;
	}

	if (fgets(buf, sizeof(buf), f) == NULL) {
		RTE_LOG(ERR, EAL, "%s(): cannot read sysfs value %s\n",
			__func__, filename);
		fclose(f);
		return -1;
	}
	*val = strtoul(buf, &end, 0);
	if ((buf[0] == '\0') || (end == NULL) || (*end != '\n')) {
		RTE_LOG(ERR, EAL, "%s(): cannot parse sysfs value %s\n",
				__func__, filename);
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}

/* parse one line of the "resource" sysfs file (note that the 'line'
 * string is modified)
 */
int
pci_parse_one_sysfs_resource(char *line, size_t len, uint64_t *phys_addr,
	uint64_t *end_addr, uint64_t *flags)
{
	union pci_resource_info {
		struct {
			char *phys_addr;
			char *end_addr;
			char *flags;
		};
		char *ptrs[PCI_RESOURCE_FMT_NVAL];
	} res_info;

	if (rte_strsplit(line, len, res_info.ptrs, 3, ' ') != 3) {
		RTE_LOG(ERR, EAL,
			"%s(): bad resource format\n", __func__);
		return -1;
	}
	errno = 0;
	*phys_addr = strtoull(res_info.phys_addr, NULL, 16);
	*end_addr = strtoull(res_info.end_addr, NULL, 16);
	*flags = strtoull(res_info.flags, NULL, 16);
	if (errno != 0) {
		RTE_LOG(ERR, EAL,
			"%s(): bad resource format\n", __func__);
		return -1;
	}

	return 0;
}

/* parse the "resource" sysfs file */
static int
pci_parse_sysfs_resource(const char *filename/*, struct rte_pci_device *dev*/)
{
	FILE *f;
	char buf[BUFSIZ];
	int i;
	uint64_t phys_addr, end_addr, flags;

	f = fopen(filename, "r");
	if (f == NULL) {
		RTE_LOG(ERR, EAL, "Cannot open sysfs resource\n");
		return -1;
	}

	for (i = 0; i<PCI_MAX_RESOURCE; i++) {

		if (fgets(buf, sizeof(buf), f) == NULL) {
			RTE_LOG(ERR, EAL,
				"%s(): cannot read resource\n", __func__);
			goto error;
		}
		if (pci_parse_one_sysfs_resource(buf, sizeof(buf), &phys_addr,
				&end_addr, &flags) < 0)
			goto error;
        
        printf("[%.30s] %ld-%ld %ld\n", filename, phys_addr, end_addr, flags);
//		if (flags & IORESOURCE_MEM) {
//			dev->mem_resource[i].phys_addr = phys_addr;
//			dev->mem_resource[i].len = end_addr - phys_addr + 1;
//			/* not mapped for now */
//			dev->mem_resource[i].addr = NULL;
//		}
	}
	fclose(f);
	return 0;

error:
	fclose(f);
	return -1;
}

static int
pci_get_kernel_driver_by_path(const char *filename, char *dri_name,
			      size_t len)
{
	int count;
	char path[PATH_MAX];
	char *name;

	if (!filename || !dri_name)
		return -1;

	count = readlink(filename, path, PATH_MAX);
	if (count >= PATH_MAX)
		return -1;

	/* For device does not have a driver */
	if (count < 0)
		return 1;

	path[count] = '\0';

	name = strrchr(path, '/');
	if (name) {
		rte_strlcpy(dri_name, name + 1, len);
		return 0;
	}

	return -1;
}

/* Scan one pci sysfs entry, and fill the devices list from it. */
static int
pci_scan_one(const char *dirname, const struct rte_pci_addr *addr)
{
	char filename[PATH_MAX];
	unsigned long tmp;
//	struct rte_pci_device *dev;
	char driver[PATH_MAX];
	int ret;

//	dev = malloc(sizeof(*dev));
//	if (dev == NULL)
//		return -1;

//	memset(dev, 0, sizeof(*dev));
//	dev->device.bus = &rte_pci_bus.bus;
//	dev->addr = *addr;

	/* get vendor id */
	snprintf(filename, sizeof(filename), "%s/vendor", dirname);
	if (eal_parse_sysfs_value(filename, &tmp) < 0) {
//		free(dev);
		return -1;
	}
//	dev->id.vendor_id = (uint16_t)tmp;
    printf("[%.30s] %ld\n", "vendor", tmp);

	/* get device id */
	snprintf(filename, sizeof(filename), "%s/device", dirname);
	if (eal_parse_sysfs_value(filename, &tmp) < 0) {
//		free(dev);
		return -1;
	}
//	dev->id.device_id = (uint16_t)tmp;
    printf("[%.30s] %ld\n", "device", tmp);

	/* get subsystem_vendor id */
	snprintf(filename, sizeof(filename), "%s/subsystem_vendor",
		 dirname);
	if (eal_parse_sysfs_value(filename, &tmp) < 0) {
//		free(dev);
		return -1;
	}
//	dev->id.subsystem_vendor_id = (uint16_t)tmp;
    printf("[%.30s] %ld\n", "subsystem_vendor", tmp);

	/* get subsystem_device id */
	snprintf(filename, sizeof(filename), "%s/subsystem_device",
		 dirname);
	if (eal_parse_sysfs_value(filename, &tmp) < 0) {
//		free(dev);
		return -1;
	}
//	dev->id.subsystem_device_id = (uint16_t)tmp;
    printf("[%.30s] %ld\n", "subsystem_device", tmp);

	/* get class_id */
	snprintf(filename, sizeof(filename), "%s/class",
		 dirname);
	if (eal_parse_sysfs_value(filename, &tmp) < 0) {
//		free(dev);
		return -1;
	}
	/* the least 24 bits are valid: class, subclass, program interface */
//	dev->id.class_id = (uint32_t)tmp & RTE_CLASS_ANY_ID;
    printf("[%.30s] %ld\n", "class", tmp);

	/* get max_vfs */
//	dev->max_vfs = 0;
	snprintf(filename, sizeof(filename), "%s/max_vfs", dirname);
	if (!access(filename, F_OK) &&
	    eal_parse_sysfs_value(filename, &tmp) == 0)
//		dev->max_vfs = (uint16_t)tmp;
        printf("[%.30s] %ld\n", "max_vfs", tmp);
	else {
		/* for non igb_uio driver, need kernel version >= 3.8 */
		snprintf(filename, sizeof(filename),
			 "%s/sriov_numvfs", dirname);
		if (!access(filename, F_OK) &&
		    eal_parse_sysfs_value(filename, &tmp) == 0)
//			dev->max_vfs = (uint16_t)tmp;
            printf("[%.30s] %ld\n", "max_vfs", tmp);
	}

	/* get numa node, default to 0 if not present */
	snprintf(filename, sizeof(filename), "%s/numa_node",
		 dirname);

	if (access(filename, F_OK) != -1) {
		if (eal_parse_sysfs_value(filename, &tmp) == 0)
//			dev->device.numa_node = tmp;
            printf("[%.30s] %ld\n", "numa_node", tmp);
		else
//			dev->device.numa_node = -1;
            printf("[%.30s] %ld\n", "numa_node", -1);
	} else {
//		dev->device.numa_node = 0;
        printf("[%.30s] %ld\n", "numa_node", 0);
	}

//	pci_name_set(dev);

	/* parse resources */
	snprintf(filename, sizeof(filename), "%s/resource", dirname);
	if (pci_parse_sysfs_resource(filename/*, dev*/) < 0) {
		RTE_LOG(ERR, EAL, "%s(): cannot parse resource\n", __func__);
//		free(dev);
		return -1;
	}

	/* parse driver */
	snprintf(filename, sizeof(filename), "%s/driver", dirname);
	ret = pci_get_kernel_driver_by_path(filename, driver, sizeof(driver));
	if (ret < 0) {
		RTE_LOG(ERR, EAL, "Fail to get kernel driver\n");
//		free(dev);
		return -1;
	}

	if (!ret) {
		if (!strcmp(driver, "vfio-pci"))
//			dev->kdrv = RTE_KDRV_VFIO;
            printf("[%.30s] %.30s\n", driver, "vfio-pci");
		else if (!strcmp(driver, "igb_uio"))
//			dev->kdrv = RTE_KDRV_IGB_UIO;
            printf("[%.30s] %.30s\n", driver, "igb_uio");
		else if (!strcmp(driver, "uio_pci_generic"))
//			dev->kdrv = RTE_KDRV_UIO_GENERIC;
            printf("[%.30s] %.30s\n", driver, "uio_pci_generic");
		else
//			dev->kdrv = RTE_KDRV_UNKNOWN;
            printf("[%.30s] %.30s\n", driver, "unknown");
	} else
//		dev->kdrv = RTE_KDRV_NONE;
        printf("[%.30s] %.30s\n", driver, "NONE");
#if 0
	/* device is valid, add in list (sorted) */
	if (TAILQ_EMPTY(&rte_pci_bus.device_list)) {
		rte_pci_add_device(dev);
	} else {
		struct rte_pci_device *dev2;
		int ret;

		TAILQ_FOREACH(dev2, &rte_pci_bus.device_list, next) {
			ret = rte_pci_addr_cmp(&dev->addr, &dev2->addr);
			if (ret > 0)
				continue;

			if (ret < 0) {
				rte_pci_insert_device(dev2, dev);
			} else { /* already registered */
				if (!rte_dev_is_probed(&dev2->device)) {
					dev2->kdrv = dev->kdrv;
					dev2->max_vfs = dev->max_vfs;
					pci_name_set(dev2);
					memmove(dev2->mem_resource,
						dev->mem_resource,
						sizeof(dev->mem_resource));
				} else {
					/**
					 * If device is plugged and driver is
					 * probed already, (This happens when
					 * we call rte_dev_probe which will
					 * scan all device on the bus) we don't
					 * need to do anything here unless...
					 **/
					if (dev2->kdrv != dev->kdrv ||
						dev2->max_vfs != dev->max_vfs)
						/*
						 * This should not happens.
						 * But it is still possible if
						 * we unbind a device from
						 * vfio or uio before hotplug
						 * remove and rebind it with
						 * a different configure.
						 * So we just print out the
						 * error as an alarm.
						 */
						RTE_LOG(ERR, EAL, "Unexpected device scan at %s!\n",
							filename);
					else if (dev2->device.devargs !=
						 dev->device.devargs) {
						rte_devargs_remove(dev2->device.devargs);
						pci_name_set(dev2);
					}
				}
				free(dev);
			}
			return 0;
		}

		rte_pci_add_device(dev);
	}
#endif
	return 0;
}


/*
 * Scan the content of the PCI bus, and the devices in the devices
 * list
 */
int
rte_pci_scan(void)
{
	struct dirent *e;
	DIR *dir;
	char dirname[PATH_MAX];
	struct rte_pci_addr addr;

	/* for debug purposes, PCI can be disabled */
//	if (!rte_eal_has_pci())
//		return 0;

//#ifdef VFIO_PRESENT
//	if (!pci_vfio_is_enabled())
//		RTE_LOG(DEBUG, EAL, "VFIO PCI modules not loaded\n");
//#endif

	dir = opendir(rte_pci_get_sysfs_path());
	if (dir == NULL) {
		RTE_LOG(ERR, EAL, "%s(): opendir failed: %s\n",
			__func__, strerror(errno));
		return -1;
	}

	while ((e = readdir(dir)) != NULL) {
		if (e->d_name[0] == '.')
			continue;

		if (parse_pci_addr_format(e->d_name, sizeof(e->d_name), &addr) != 0)
			continue;

		if (rte_pci_ignore_device(&addr))
			continue;

		snprintf(dirname, sizeof(dirname), "%s/%s",
				rte_pci_get_sysfs_path(), e->d_name);

		if (pci_scan_one(dirname, &addr) < 0)
			goto error;
	}
	closedir(dir);
	return 0;

error:
	closedir(dir);
	return -1;
}

int main()
{
    rte_pci_scan();
}

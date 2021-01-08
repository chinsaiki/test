#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <linux/vfio.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define VFIO_DEVICE_FLAGS_DEVTREE	(1 << 3) /* device tree metadata */

struct vfio_devtree_info {
	__u32	argsz;
	__u32	type;
#define VFIO_DEVTREE_PROP_NAMES		0
#define VFIO_DEVTREE_ARR_TYPE_STRING	1
#define VFIO_DEVTREE_ARR_TYPE_U8	2
#define VFIO_DEVTREE_ARR_TYPE_U16	3
#define VFIO_DEVTREE_ARR_TYPE_U32	4
	__u32	length;
	__u8	data[];
};
#define VFIO_DEVICE_GET_DEVTREE_INFO	_IO(VFIO_TYPE, VFIO_BASE + 17)

static void vfio_pr_devtree_prop(int device, char *name, unsigned int type)
{
	static unsigned int length = 0;
	static struct vfio_devtree_info *devtree = NULL;
	int ret;

	if (length < strlen(name) + 1)
		length = strlen(name) + 1;

	while (1) {
		unsigned int argsz = sizeof(struct vfio_devtree_info) + length;
		devtree = realloc(devtree, argsz);
		devtree->argsz = argsz;
		devtree->type = type;
		strcpy(devtree->data, name);

		ret = ioctl(device, VFIO_DEVICE_GET_DEVTREE_INFO, devtree);

		if (length < devtree->length)
			length = devtree->length;
		else
			break;
	}

	if (ret) {
		printf("%s = error %d\n", name, ret);
	} else if (type == VFIO_DEVTREE_ARR_TYPE_STRING ||
			type == VFIO_DEVTREE_PROP_NAMES) {
		int i;
		printf("%s =", name);
		for (i=0; i < devtree->length; i += strlen(devtree->data + i) + 1)
			printf(" \"%s\"", devtree->data + i);
		printf("\n");

	} else if (type == VFIO_DEVTREE_ARR_TYPE_U32 ||
		   type == VFIO_DEVTREE_ARR_TYPE_U16 ||
		   type == VFIO_DEVTREE_ARR_TYPE_U8) {
		long unsigned int *uarr = (long unsigned int *) devtree->data;

		printf("%s =", name);
		while (uarr < devtree->data + devtree->length) {
			printf(" 0x%lx", *uarr);
			uarr++;
		}
		printf("\n");
	}
}

int main (int argc, char **argv) {

	int container, group, device;
	unsigned int i;

	struct vfio_group_status group_status = { .argsz = sizeof(group_status) };
	struct vfio_iommu_type1_info iommu_info = { .argsz = sizeof(iommu_info) };
	struct vfio_iommu_type1_dma_map dma_map = { .argsz = sizeof(dma_map) };
	struct vfio_device_info device_info = { .argsz = sizeof(device_info) };

	int ret;

	if (argc != 3) {
		printf("Usage: ./vfio-dt /dev/vfio/${group_id} device_id\n");
		return 2;
	}

	/* Create a new container */
	container = open("/dev/vfio/vfio", O_RDWR);

	if (ioctl(container, VFIO_GET_API_VERSION) != VFIO_API_VERSION) {
		printf("Unknown API version\n");
		return 1;
	}

	if (!ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU)) {
		printf("Doesn't support the IOMMU driver we want\n");
		return 1;
	}

	/* Open the group */
	group = open(argv[1], O_RDWR);

	/* Test the group is viable and available */
	ioctl(group, VFIO_GROUP_GET_STATUS, &group_status);

	if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
		printf("Group is not viable (not all devices bound for vfio)\n");
		return 1;
	}

	/* Add the group to the container */
	ioctl(group, VFIO_GROUP_SET_CONTAINER, &container);

	/* Enable the IOMMU model we want */
	ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);

	/* Get addition IOMMU info */
	ioctl(container, VFIO_IOMMU_GET_INFO, &iommu_info);

	/* Get a file descriptor for the device */
	device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, argv[2]);
	printf("=== VFIO device file descriptor %d ===\n", device);

	/* Test and setup the device */
	ret = ioctl(device, VFIO_DEVICE_GET_INFO, &device_info);

	if(ret) {
		printf("Could not get VFIO device\n");
		return 1;
	}

	/* devtree tests */

	/* Get available property names */
	vfio_pr_devtree_prop(device, "props", VFIO_DEVTREE_PROP_NAMES);

	/* these should return an error */
	vfio_pr_devtree_prop(device, "invalid", VFIO_DEVTREE_ARR_TYPE_STRING);
	vfio_pr_devtree_prop(device, "invalid", VFIO_DEVTREE_ARR_TYPE_U32);

	/* fetch some strings */
	vfio_pr_devtree_prop(device, "name", VFIO_DEVTREE_ARR_TYPE_STRING);
	vfio_pr_devtree_prop(device, "clock-names", VFIO_DEVTREE_ARR_TYPE_STRING);
	vfio_pr_devtree_prop(device, "compatible", VFIO_DEVTREE_ARR_TYPE_STRING);

	/* u32 arrays */
	vfio_pr_devtree_prop(device, "reg", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "interrupts", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "phandle", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "linux,phandle", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "clocks", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "#dma-cells", VFIO_DEVTREE_ARR_TYPE_U32);
	vfio_pr_devtree_prop(device, "#stream-id-cells", VFIO_DEVTREE_ARR_TYPE_U32);

	/* VFIO also exposes full_name which includes the path. Should only
	 * work as a string. */
	vfio_pr_devtree_prop(device, "full_name", VFIO_DEVTREE_ARR_TYPE_STRING);
	vfio_pr_devtree_prop(device, "full_name", VFIO_DEVTREE_ARR_TYPE_U32);

	/* the pl330 device tree doesn't include 16 and 8 bit types, so
	 * a proper test cannot be done for now */
	/* get reg but as 16 bit type */
	vfio_pr_devtree_prop(device, "reg", VFIO_DEVTREE_ARR_TYPE_U16);
	/* get reg but as 8 bit type */
	vfio_pr_devtree_prop(device, "reg", VFIO_DEVTREE_ARR_TYPE_U8);

	return 0;
}

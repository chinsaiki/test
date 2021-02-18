/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

//#include <rte_log.h>
//#include <rte_eal.h>
//#include <rte_lcore.h>
//#include <rte_common.h>
//#include <rte_string_fns.h>
//#include <rte_debug.h>
//
//#include "eal_private.h"
//#include "eal_filesystem.h"
//#include "eal_thread.h"
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef RTE_MAX_NUMA_NODES
#define RTE_MAX_NUMA_NODES 4
#endif

#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)


#define SYS_CPU_DIR "/sys/devices/system/cpu/cpu%u"
#define CORE_ID_FILE "topology/core_id"
#define NUMA_NODE_PATH "/sys/devices/system/node"

/* Check if a cpu is present by the presence of the cpu information for it */
int
eal_cpu_detected(unsigned lcore_id)
{
	char path[PATH_MAX];
	int len = snprintf(path, sizeof(path), SYS_CPU_DIR
		"/"CORE_ID_FILE, lcore_id);
	if (len <= 0 || (unsigned)len >= sizeof(path))
		return 0;
	if (access(path, F_OK) != 0)
		return 0;

	return 1;
}

/*
 * Get CPU socket id (NUMA node) for a logical core.
 *
 * This searches each nodeX directories in /sys for the symlink for the given
 * lcore_id and returns the numa node where the lcore is found. If lcore is not
 * found on any numa node, returns zero.
 */
unsigned
eal_cpu_socket_id(unsigned lcore_id)
{
	unsigned socket;

	for (socket = 0; socket < RTE_MAX_NUMA_NODES; socket++) {
		char path[PATH_MAX];

		snprintf(path, sizeof(path), "%s/node%u/cpu%u", NUMA_NODE_PATH,
				socket, lcore_id);
		if (access(path, F_OK) == 0)
			return socket;
	}
	return 0;
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

/* Get the cpu core id value from the /sys/.../cpuX core_id value */
unsigned
eal_cpu_core_id(unsigned lcore_id)
{
	char path[PATH_MAX];
	unsigned long id;

	int len = snprintf(path, sizeof(path), SYS_CPU_DIR "/%s", lcore_id, CORE_ID_FILE);
	if (len <= 0 || (unsigned)len >= sizeof(path))
		goto err;
	if (eal_parse_sysfs_value(path, &id) != 0)
		goto err;
	return (unsigned)id;

err:
	RTE_LOG(ERR, EAL, "Error reading core id value from %s "
			"for lcore %u - assuming core 0\n", SYS_CPU_DIR, lcore_id);
	return 0;
}

int main()
{
    printf("CPU %d belongs to %d\n", 1, eal_cpu_socket_id(1));
}

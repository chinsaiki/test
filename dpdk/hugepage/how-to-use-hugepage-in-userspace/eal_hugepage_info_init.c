#include "common.h"
#include "hugepage_info.h"


struct internal_config internal_config; /**< Global EAL configuration. */


/*
 * when we initialize the hugepage info, everything goes
 * to socket 0 by default. it will later get sorted by memory
 * initialization procedure.
 */
int
eal_hugepage_info_init(void)
{
	struct hugepage_info *hpi, *tmp_hpi;
	unsigned int i;

	if (hugepage_info_init() < 0)
		return -1;

	/* for no shared files mode, we're done */
//	if (internal_config.no_shconf)
//		return 0;

	hpi = &internal_config.hugepage_info[0];

	tmp_hpi = create_shared_memory(eal_hugepage_info_path(),
			sizeof(internal_config.hugepage_info));
	if (tmp_hpi == NULL) {
		RTE_LOG(ERR, EAL, "Failed to create shared memory!\n");
		return -1;
	}

	memcpy(tmp_hpi, hpi, sizeof(internal_config.hugepage_info));

	/* we've copied file descriptors along with everything else, but they
	 * will be invalid in secondary process, so overwrite them
	 */
	for (i = 0; i < RTE_DIM(internal_config.hugepage_info); i++) {
		struct hugepage_info *tmp = &tmp_hpi[i];
		tmp->lock_descriptor = -1;
	}

	if (munmap(tmp_hpi, sizeof(internal_config.hugepage_info)) < 0) {
		RTE_LOG(ERR, EAL, "Failed to unmap shared memory!\n");
		return -1;
	}
	return 0;
}



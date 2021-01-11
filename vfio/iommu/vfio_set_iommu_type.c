
int
vfio_get_iommu_type(void)
{
	if (default_vfio_cfg->vfio_iommu_type == NULL)
		return -1;

	return default_vfio_cfg->vfio_iommu_type->type_id;
}

const struct vfio_iommu_type *
vfio_set_iommu_type(int vfio_container_fd)
{
	unsigned idx;
	for (idx = 0; idx < RTE_DIM(iommu_types); idx++) {
		const struct vfio_iommu_type *t = &iommu_types[idx];

		int ret = ioctl(vfio_container_fd, VFIO_SET_IOMMU,
				t->type_id);
		if (!ret) {
			RTE_LOG(NOTICE, EAL, "  using IOMMU type %d (%s)\n",
					t->type_id, t->name);
			return t;
		}
		/* not an error, there may be more supported IOMMU types */
		RTE_LOG(DEBUG, EAL, "  set IOMMU type %d (%s) failed, "
				"error %i (%s)\n", t->type_id, t->name, errno,
				strerror(errno));
	}
	/* if we didn't find a suitable IOMMU type, fail */
	return NULL;
}


#include "common.h"


#if defined(RTE_ARCH_ARM) || defined(RTE_ARCH_ARM64)
#define MAX_HUGEPAGE_SIZES 4  /**< support up to 4 page sizes */
#else
#define MAX_HUGEPAGE_SIZES 3  /**< support up to 3 page sizes */
#endif


static const char sys_dir_path[] = "/sys/kernel/mm/hugepages";
static const char sys_pages_numa_dir_path[] = "/sys/devices/system/node";


/**
 * Converts a numeric string to the equivalent uint64_t value.
 * As well as straight number conversion, also recognises the suffixes
 * k, m and g for kilobytes, megabytes and gigabytes respectively.
 *
 * If a negative number is passed in  i.e. a string with the first non-black
 * character being "-", zero is returned. Zero is also returned in the case of
 * an error with the strtoull call in the function.
 *
 * @param str
 *     String containing number to convert.
 * @return
 *     Number.
 */
static inline uint64_t
rte_str_to_size(const char *str)
{
	char *endptr;
	unsigned long long size;

	while (isspace((int)*str))
		str++;
	if (*str == '-')
		return 0;

	errno = 0;
	size = strtoull(str, &endptr, 0);
	if (errno)
		return 0;

	if (*endptr == ' ')
		endptr++; /* allow 1 space gap */

	switch (*endptr){
	case 'G': case 'g': size *= 1024; /* fall-through */
	case 'M': case 'm': size *= 1024; /* fall-through */
	case 'K': case 'k': size *= 1024; /* fall-through */
	default:
		break;
	}
	return size;
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

static uint64_t
get_default_hp_size(void)
{
	const char proc_meminfo[] = "/proc/meminfo";
	const char str_hugepagesz[] = "Hugepagesize:";
	unsigned hugepagesz_len = sizeof(str_hugepagesz) - 1;
	char buffer[256];
	unsigned long long size = 0;

	FILE *fd = fopen(proc_meminfo, "r");
	if (fd == NULL)
		rte_panic("Cannot open %s\n", proc_meminfo);
	while(fgets(buffer, sizeof(buffer), fd)){
		if (strncmp(buffer, str_hugepagesz, hugepagesz_len) == 0){
			size = rte_str_to_size(&buffer[hugepagesz_len]);
			break;
		}
	}
	fclose(fd);
	if (size == 0)
		rte_panic("Cannot get default hugepage size from %s\n", proc_meminfo);
	return size;
}

static int
get_hugepage_dir(uint64_t hugepage_sz, char *hugedir, int len)
{
	enum proc_mount_fieldnames {
		DEVICE = 0,
		MOUNTPT,
		FSTYPE,
		OPTIONS,
		_FIELDNAME_MAX
	};
	static uint64_t default_size = 0;
	const char proc_mounts[] = "/proc/mounts";
	const char hugetlbfs_str[] = "hugetlbfs";
	const size_t htlbfs_str_len = sizeof(hugetlbfs_str) - 1;
	const char pagesize_opt[] = "pagesize=";
	const size_t pagesize_opt_len = sizeof(pagesize_opt) - 1;
	const char split_tok = ' ';
	char *splitstr[_FIELDNAME_MAX];
	char buf[BUFSIZ];
	int retval = -1;

	FILE *fd = fopen(proc_mounts, "r");
	if (fd == NULL)
		rte_panic("Cannot open %s\n", proc_mounts);

	if (default_size == 0)
		default_size = get_default_hp_size();

	while (fgets(buf, sizeof(buf), fd)){
		if (rte_strsplit(buf, sizeof(buf), splitstr, _FIELDNAME_MAX,
				split_tok) != _FIELDNAME_MAX) {
			RTE_LOG(ERR, EAL, "Error parsing %s\n", proc_mounts);
			break; /* return NULL */
		}

		/* we have a specified --huge-dir option, only examine that dir */
		if (internal_config.hugepage_dir != NULL &&
				strcmp(splitstr[MOUNTPT], internal_config.hugepage_dir) != 0)
			continue;

		if (strncmp(splitstr[FSTYPE], hugetlbfs_str, htlbfs_str_len) == 0){
			const char *pagesz_str = strstr(splitstr[OPTIONS], pagesize_opt);

			/* if no explicit page size, the default page size is compared */
			if (pagesz_str == NULL){
				if (hugepage_sz == default_size){
					strlcpy(hugedir, splitstr[MOUNTPT], len);
					retval = 0;
					break;
				}
			}
			/* there is an explicit page size, so check it */
			else {
				uint64_t pagesz = rte_str_to_size(&pagesz_str[pagesize_opt_len]);
				if (pagesz == hugepage_sz) {
					strlcpy(hugedir, splitstr[MOUNTPT], len);
					retval = 0;
					break;
				}
			}
		} /* end if strncmp hugetlbfs */
	} /* end while fgets */

	fclose(fd);
	return retval;
}

/*
 * Clear the hugepage directory of whatever hugepage files
 * there are. Checks if the file is locked (i.e.
 * if it's in use by another DPDK process).
 */
static int
clear_hugedir(const char * hugedir)
{
	DIR *dir;
	struct dirent *dirent;
	int dir_fd, fd, lck_result;
	const char filter[] = "*map_*"; /* matches hugepage files */

	/* open directory */
	dir = opendir(hugedir);
	if (!dir) {
		RTE_LOG(ERR, EAL, "Unable to open hugepage directory %s\n",
				hugedir);
		goto error;
	}
	dir_fd = dirfd(dir);

	dirent = readdir(dir);
	if (!dirent) {
		RTE_LOG(ERR, EAL, "Unable to read hugepage directory %s\n",
				hugedir);
		goto error;
	}

	while(dirent != NULL){
		/* skip files that don't match the hugepage pattern */
		if (fnmatch(filter, dirent->d_name, 0) > 0) {
			dirent = readdir(dir);
			continue;
		}

		/* try and lock the file */
		fd = openat(dir_fd, dirent->d_name, O_RDONLY);

		/* skip to next file */
		if (fd == -1) {
			dirent = readdir(dir);
			continue;
		}

		/* non-blocking lock */
		lck_result = flock(fd, LOCK_EX | LOCK_NB);

		/* if lock succeeds, remove the file */
		if (lck_result != -1)
			unlinkat(dir_fd, dirent->d_name, 0);
		close (fd);
		dirent = readdir(dir);
	}

	closedir(dir);
	return 0;

error:
	if (dir)
		closedir(dir);

	RTE_LOG(ERR, EAL, "Error while clearing hugepage dir: %s\n",
		strerror(errno));

	return -1;
}


static int
hugepage_info_init(void)
{	const char dirent_start_text[] = "hugepages-";
	const size_t dirent_start_len = sizeof(dirent_start_text) - 1;
	unsigned int i, num_sizes = 0;
	DIR *dir;
	struct dirent *dirent;

	dir = opendir(sys_dir_path);
	if (dir == NULL) {
		RTE_LOG(ERR, EAL,
			"Cannot open directory %s to read system hugepage info\n",
			sys_dir_path);
		return -1;
	}

	for (dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
		struct hugepage_info *hpi;

		if (strncmp(dirent->d_name, dirent_start_text,
			    dirent_start_len) != 0)
			continue;

		if (num_sizes >= MAX_HUGEPAGE_SIZES)
			break;

		hpi = &internal_config.hugepage_info[num_sizes];
		hpi->hugepage_sz =
			rte_str_to_size(&dirent->d_name[dirent_start_len]);

		/* first, check if we have a mountpoint */
		if (get_hugepage_dir(hpi->hugepage_sz,
			hpi->hugedir, sizeof(hpi->hugedir)) < 0) {
			uint32_t num_pages;

			num_pages = get_num_hugepages(dirent->d_name);
			if (num_pages > 0)
				RTE_LOG(NOTICE, EAL,
					"%" PRIu32 " hugepages of size "
					"%" PRIu64 " reserved, but no mounted "
					"hugetlbfs found for that size\n",
					num_pages, hpi->hugepage_sz);
			/* if we have kernel support for reserving hugepages
			 * through mmap, and we're in in-memory mode, treat this
			 * page size as valid. we cannot be in legacy mode at
			 * this point because we've checked this earlier in the
			 * init process.
			 */
#ifdef MAP_HUGE_SHIFT
			if (internal_config.in_memory) {
				RTE_LOG(DEBUG, EAL, "In-memory mode enabled, "
					"hugepages of size %" PRIu64 " bytes "
					"will be allocated anonymously\n",
					hpi->hugepage_sz);
				calc_num_pages(hpi, dirent);
				num_sizes++;
			}
#endif
			continue;
		}

		/* try to obtain a writelock */
		hpi->lock_descriptor = open(hpi->hugedir, O_RDONLY);

		/* if blocking lock failed */
		if (flock(hpi->lock_descriptor, LOCK_EX) == -1) {
			RTE_LOG(CRIT, EAL,
				"Failed to lock hugepage directory!\n");
			break;
		}
		/* clear out the hugepages dir from unused pages */
		if (clear_hugedir(hpi->hugedir) == -1)
			break;

		calc_num_pages(hpi, dirent);

		num_sizes++;
	}
	closedir(dir);

	/* something went wrong, and we broke from the for loop above */
	if (dirent != NULL)
		return -1;

	internal_config.num_hugepage_sizes = num_sizes;

	/* sort the page directory entries by size, largest to smallest */
	qsort(&internal_config.hugepage_info[0], num_sizes,
	      sizeof(internal_config.hugepage_info[0]), compare_hpi);

	/* now we have all info, check we have at least one valid size */
	for (i = 0; i < num_sizes; i++) {
		/* pages may no longer all be on socket 0, so check all */
		unsigned int j, num_pages = 0;
		struct hugepage_info *hpi = &internal_config.hugepage_info[i];

		for (j = 0; j < RTE_MAX_NUMA_NODES; j++)
			num_pages += hpi->num_pages[j];
		if (num_pages > 0)
			return 0;
	}

	/* no valid hugepage mounts available, return error */
	return -1;
}


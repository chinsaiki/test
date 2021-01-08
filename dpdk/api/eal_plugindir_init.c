#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


#ifndef PATH_MAX
#define PATH_MAX 256
#endif
#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)


static int
eal_plugin_add(const char *path)
{
	printf("eal_plugin_add %s\n", path);

	return 0;
}

static int
eal_plugindir_init(const char *path)
{
	DIR *d = NULL;
	struct dirent *dent = NULL;
	char sopath[PATH_MAX];

	if (path == NULL || *path == '\0')
		return 0;

	d = opendir(path);
	if (d == NULL) {
		RTE_LOG(ERR, EAL, "failed to open directory %s: %s\n",
			path, strerror(errno));
		return -1;
	}

	while ((dent = readdir(d)) != NULL) {
		struct stat sb;

		snprintf(sopath, sizeof(sopath), "%s/%s", path, dent->d_name);

		if (!(stat(sopath, &sb) == 0 && S_ISREG(sb.st_mode)))
			continue;

		if (eal_plugin_add(sopath) == -1)
			break;
	}

	closedir(d);
	/* XXX this ignores failures from readdir() itself */
	return (dent == NULL) ? 0 : -1;
}


int main()
{
    eal_plugindir_init("./");
}


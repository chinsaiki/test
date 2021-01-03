#include <dirent.h>

//scan a directory for matching entries
int scandir(const char *dirp, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **));

//int alphasort(const void *a, const void *b);

//int versionsort(const void *a, const void *b);


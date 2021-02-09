/* Copyright (c) Colorado School of Mines, CST.*/
/* All rights reserved.                       */

#include <stdio.h>
#include <time.h>
#include <errno.h>

#define TRACE_PREFIX_LEN 12
#define TRACE_DIR_STR_LEN (sizeof("YYYY-mm-dd-AM-HH-MM-SS") + TRACE_PREFIX_LEN)
#define TRACE_CTF_FIELD_SIZE 384
#define TRACE_POINT_NAME_SIZE 64
#define TRACE_CTF_MAGIC 0xC1FC1FC1
#define TRACE_MAX_ARGS	32


__thread int rte_errno;

/* Copy src string into dst.
 *
 * Return negative value and NUL-terminate if dst is too short,
 * Otherwise return number of bytes copied.
 */
ssize_t
rte_strscpy(char *dst, const char *src, size_t dsize)
{
	size_t nleft = dsize;
	size_t res = 0;

	/* Copy as many bytes as will fit. */
	while (nleft != 0) {
		dst[res] = src[res];
		if (src[res] == '\0')
			return res;
		res++;
		nleft--;
	}

	/* Not enough room in dst, set NUL and return error. */
	if (res != 0)
		dst[res - 1] = '\0';
	return -E2BIG;
}

const char *
eal_get_hugefile_prefix(void)
{
//	if (internal_config.hugefile_prefix != NULL)
//		return internal_config.hugefile_prefix;
	return "rte";
}


static int
trace_session_name_generate(char *trace_dir)
{
	struct tm *tm_result;
	time_t tm;
	int rc;

	tm = time(NULL);
	if ((int)tm == -1)
		goto fail;

	tm_result = localtime(&tm);
	if (tm_result == NULL)
		goto fail;

	rc = rte_strscpy(trace_dir, eal_get_hugefile_prefix(),
			TRACE_PREFIX_LEN);
	if (rc == -E2BIG)
		rc = TRACE_PREFIX_LEN;
	trace_dir[rc++] = '-';

	rc = strftime(trace_dir + rc, TRACE_DIR_STR_LEN - rc,
			"%Y-%m-%d-%p-%I-%M-%S", tm_result);
	if (rc == 0)
		goto fail;

	return rc;
fail:
	rte_errno = errno;
	return -rte_errno;
}

int main()
{
    char str[256] = {0};
    trace_session_name_generate(str);

    printf("str = %s\n", str);
}

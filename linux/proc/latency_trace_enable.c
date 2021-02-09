#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>



/* Write the entirety of data.  Complain if unable to do so. */
static void write_or_complain(int fd, const void *data, size_t len)
{
	const char *remaining = data;

	while (len > 0) {
		ssize_t ret = write(fd, remaining, len);
		if (ret <= 0) {
			if (errno != EAGAIN && errno != EINTR) {
				perror("write");
				return;
			}
		} else {
			remaining += ret;
			len -= ret;
		}
	}
}

/* Write the given data to the existing file specified by pathname.  Complain
 * if unable to do so. */
static void write_file(const char *pathname, const void *data, size_t len)
{
	int fd = open(pathname, O_WRONLY);
	if (fd < 0) {
		printf("Failed to open file \"%s\": %d (%s)\n",
		       pathname, errno, strerror(errno));
		return;
	}

	write_or_complain(fd, data, len);

	if (close(fd) < 0) {
		printf("Failed to close file \"%s\": %d (%s)\n",
		       pathname, errno, strerror(errno));
	}
}

/* Write the given '\0'-terminated string to the existing file specified by
 * pathname.  Complain if unable to do so. */
static void write_string_to_file(const char *pathname, const char *string)
{
	write_file(pathname, string, strlen(string));
}

#ifndef PR_SET_TRACING
#define PR_SET_TRACING 0
#endif

void latency_trace_start(void)
{
	if (prctl(PR_SET_TRACING, 1) < 0)
		perror("Failed to start tracing");
}

void latency_trace_stop(void)
{
	if (prctl(PR_SET_TRACING, 0) < 0)
		perror("Failed to stop tracing");
}


static void read_and_print(const char *pathname, int output_fd)
{
	char data[4096];
	int fd = open(pathname, O_RDONLY);
	if (fd < 0) {
		printf("Failed to open file \"%s\": %d (%s)\n",
		       pathname, errno, strerror(errno));
		return;
	}

	while (1) {
		ssize_t ret = read(fd, data, sizeof(data));
		if (ret < 0) {
			if (errno != EAGAIN && errno != EINTR) {
				printf
				    ("Failed to read from file \"%s\": %d (%s)\n",
				     pathname, errno, strerror(errno));
				break;
			}
		} else if (ret == 0)
			break;
		else
			write_or_complain(output_fd, data, ret);
	}

	if (close(fd) < 0) {
		printf("Failed to close file \"%s\": %d (%s)\n",
		       pathname, errno, strerror(errno));
	}
}

void latency_trace_print(void)
{
	read_and_print("/proc/latency_trace", STDOUT_FILENO);
}


void latency_trace_enable(void)
{
	printf("Enabling latency tracer.\n");
	write_string_to_file("/proc/sys/kernel/trace_use_raw_cycles", "1");
	write_string_to_file("/proc/sys/kernel/trace_all_cpus", "1");
	write_string_to_file("/proc/sys/kernel/trace_enabled", "1");
	write_string_to_file("/proc/sys/kernel/trace_freerunning", "1");
	write_string_to_file("/proc/sys/kernel/trace_print_on_crash", "0");
	write_string_to_file("/proc/sys/kernel/trace_user_triggered", "1");
	write_string_to_file("/proc/sys/kernel/trace_user_trigger_irq", "-1");
	write_string_to_file("/proc/sys/kernel/trace_verbose", "0");
	write_string_to_file("/proc/sys/kernel/preempt_thresh", "0");
	write_string_to_file("/proc/sys/kernel/wakeup_timing", "0");
	write_string_to_file("/proc/sys/kernel/mcount_enabled", "1");
	write_string_to_file("/proc/sys/kernel/preempt_max_latency", "0");
}


int main()
{
    
    latency_trace_enable();
    latency_trace_start();

    return 0;
}

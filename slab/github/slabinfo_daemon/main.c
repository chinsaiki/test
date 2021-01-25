#include "slabinfo.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>

static char log_file_name[] = "/var/log/slabinfo_daemon.log";
static char pid_file_name[] = "/var/run/slabinfo_daemon.pid";

static FILE* log_stream;

static int running = 0;
static int pid_fd = -1;


void handle_sigint(int sig) {
    if (pid_fd != -1) {
        lockf(pid_fd, F_ULOCK, 0);
        close(pid_fd);
    }
    unlink(pid_file_name);
    running = 0;
    signal(SIGINT, SIG_DFL);
}

void do_fork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Can't fork");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
}

void become_session_leader() {
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }
}

void change_file_creation_mask() {
    umask(0);
}

void change_workdir_to_root() {
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }
}

void close_open_fd() {
    for (long fd = sysconf(_SC_OPEN_MAX); fd > 0; --fd) {
        close((int) fd);
    }
}

void open_std_to_dev_null() {
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");
}

void ensure_single_running_instance() {
    char str[256];
    pid_fd = open(pid_file_name, O_RDWR | O_CREAT, 0640);
    if (pid_fd < 0) {
        exit(EXIT_FAILURE);
    }
    if (lockf(pid_fd, F_TLOCK, 0) < 0) {
        syslog(LOG_ERR, "Slabinfo daemon is already running");
        exit(EXIT_FAILURE);
    }
    sprintf(str, "%d\n", getpid());
    write(pid_fd, str, strlen(str));
}

void daemonize(void) {
    do_fork();
    become_session_leader();
    signal(SIGCHLD, SIG_IGN);
    change_file_creation_mask();
    change_workdir_to_root();
    open_std_to_dev_null();
    ensure_single_running_instance();
}

int main(int argc, char *argv[]) {
    daemonize();

    openlog(argv[0], LOG_PID | LOG_CONS, LOG_DAEMON);
    signal(SIGINT, handle_sigint);

    running = 1;
    while (running == 1) {
        log_stream = fopen(log_file_name, "a+");
        if (log_stream == NULL) {
            syslog(LOG_ERR, "Can not open log file: %s, error: %s", log_file_name, strerror(errno));
            log_stream = stdout;
        }

        slabinfo_cycle(log_stream);

        fclose(log_stream);
    }

    closelog();

    return EXIT_SUCCESS;
}

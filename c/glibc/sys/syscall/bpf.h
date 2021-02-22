#include <unistd.h>
#include <linux/bpf.h>

int bpf(int cmd, union bpf_attr *attr, unsigned int size);

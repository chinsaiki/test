#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/version.h>

#define KNI_DEVICE "kni"
#define RTE_LOG(MOD, LEVEL, fmt...) printf(fmt)

static volatile int kni_fd = -1;
/* Shall be called before any allocation happens */
int
rte_kni_init(unsigned int max_kni_ifaces)
{

    /* Check FD and open */
    if (kni_fd < 0) {
        kni_fd = open("/dev/" KNI_DEVICE, O_RDWR);
        if (kni_fd < 0) {
            RTE_LOG(ERR, KNI,
                "Can not open /dev/%s\n", KNI_DEVICE);
            return -1;
        }
    }
}

int main()
{
    rte_kni_init(1);
}

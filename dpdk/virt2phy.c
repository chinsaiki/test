// Convert virtual address to physical address
// https://www.kernel.org/doc/Documentation/vm/pagemap.txt
static uintptr_t virt_to_phys(void* virt) {
    long pagesize = sysconf(_SC_PAGESIZE);
    int fd = open("/proc/self/pagemap", O_RDONLY);
    ASSERT(fd != -1, "failed to open /proc/self/pagemap");
    off_t ret =
        lseek(fd, (uintptr_t)virt / pagesize * sizeof(uintptr_t), SEEK_SET);
    ASSERT(ret != -1, "lseek error");
    uintptr_t entry = 0;
    ssize_t rc = read(fd, &entry, sizeof(entry));
    ASSERT(rc > 0, "read error");
    ASSERT(entry != 0,
           "failed to get physical address for %p (perhaps forgot sudo?)",
           virt);
    close(fd);

    return (entry & 0x7fffffffffffffULL) * pagesize +
           ((uintptr_t)virt) % pagesize;
}


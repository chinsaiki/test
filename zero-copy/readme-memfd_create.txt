创建匿名文件


------------------------------------------------
*** test/c/glibc/sys/syscall  [syscall(__NR_memfd_create, "elf", MFD_CLOEXEC);]
*** test/c/glibc/sys/mman [memfd_create(name, MFD_ALLOW_SEALING);]
*** test/c/glibc/sys/mman/memfd_create [示例程序]
*** test/ipc/socket/unsocket-sendmsg-iovec 进程间通过UNXI socket实现fd共享
*** test/ipc/socket/unsocket-sendmsg-iovec-memfd_create 进程间fd共享内存


------------------------------------------------
*** test/ipc/socket/unsocket-sendmsg-iovec

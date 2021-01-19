futex - 快速的健壮的用户空间互斥

semaphore 必须通过 semop(2) 进入内核进行PV操作，开销很大
1. 保存寄存器状态
2. 从用户态切换到内核态，等
通常需要上百条指令，事实上，有一部分代码是可以避免的。

futex 的解决思路是： 在无竞争的情况下操作完全在 用户空间 进行，
不需要系统调用，仅在发生竞争的时候进入内核。所以说 futex 是内核态和
用户态混合的同步机制，需要两种模式合作完成。
futex 变量必须位于用户空间。

----------------------------------------------
*** test/linux-5.6.5/tools/testing/selftests/futex
*** test/c/glibc/linux/futex
*** test/futex
*** test/futex/github/exploitingFutex  一个基于 MMAP 的
*** test/futex/github/Futex		两个简单的 main.c 测试 futex 锁
*** test/futex/github/futex-exp		futex 的 FUTEX_REQUEUE 机制
*** test/futex/github/obqueue	高效简单的并发队列
*** 

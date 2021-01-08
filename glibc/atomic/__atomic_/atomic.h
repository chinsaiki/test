//GCC实现了C++11之后（gcc版本要大于4.7），上面的__sync系列操作就不被推荐使用了，
//而基于C++11的新原子操作接口使用__atomic作为前缀。
//与__sync系列相比，__atomic系列函数最大的不同就是，需要指定内存序。

#define __ATOMIC_RELAXED    //编译器和处理器可以对memory access做任何的reorder
#define __ATOMIC_ACQUIRE    //保证本线程中,所有后续的读操作必须在本条原子操作完成后执行。
#define __ATOMIC_RELEASE    //保证本线程中,所有之前的写操作完成后才能执行本条原子操作。
#define __ATOMIC_ACQ_REL    //同时包含 memory_order_acquire 和 memory_order_release
#define __ATOMIC_CONSUME    //释放消费顺序（release/consume）的规范正在修订中，而且暂时不鼓励使用 
#define __ATOMIC_SEQ_CST    //最强的同步模式，同时也是默认的模型，具有强烈的happens-before语义


std::memory_order_relaxed(__ATOMIC_RELAXED);
//不对执行顺序做保证，没有happens-before的约束，编译器和处理器可以对memory access做任何的reorder，
//这种模式下能做的唯一保证，就是一旦线程读到了变量var的最新值，那么这个线程将再也见不到var修改之前的值了。
//同时当relaxed存一个数据的时候，另外的线程将需要一个时间才能relaxed读到该值，在非缓存一致性的构架上需要刷新缓存。
//在开发的时候，如果你的上下文没有共享的变量需要在线程间同步，选用Relaxed就可以了。

std::memory_order_acquire(__ATOMIC_ACQUIRE);
std::memory_order_release(__ATOMIC_RELEASE);
//memory_order_acquire保证本线程中,所有后续的读操作必须在本条原子操作完成后执行。
//memory_order_release保证本线程中,所有之前的写操作完成后才能执行本条原子操作。
//通常的做法是：
//    将资源通过store+memory_order_release的方式”Release”给别的线程；
//    别的线程则通过load+memory_order_acquire判断或者等待某个资源，
//    一旦满足某个条件后就可以安全的“Acquire”消费这些资源了。即释放获得顺序。

std::memory_order_acq_rel(__ATOMIC_ACQ_REL);
//    memory_order_acq_rel则是同时包含memory_order_acquire 和 memory_order_release

std::memory_order_consume(__ATOMIC_CONSUME);
//释放消费顺序（release/consume）的规范正在修订中，而且暂时不鼓励使用 memory_order_consume 。

std::memory_order_seq_cst(__ATOMIC_SEQ_CST);
//该模型是最强的同步模式，同时也是默认的模型，具有强烈的happens-before语义，
//来保证指令的顺序一致执行，相当于不打开编译器优化指令，按照正常的指令序执行。
//多线程各原子操作也会Synchronized-with，譬如atomic::load()需要等待atomic::store()写下元素才能读取。
//更进一步地，读操作需要在“一个写操作对所有处理器可见”的时候才能读，适用于基于缓存的体系结构。



type __atomic_add_fetch(type *ptr, type val, int memorder);
type __atomic_sub_fetch(type *ptr, type val, int memorder);
type __atomic_and_fetch(type *ptr, type val, int memorder);
type __atomic_xor_fetch(type *ptr, type val, int memorder);
type __atomic_or_fetch(type *ptr, type val, int memorder);
type __atomic_nand_fetch(type *ptr, type val, int memorder);

type __atomic_fetch_add(type *ptr, type val, int memorder);
type __atomic_fetch_sub(type *ptr, type val, int memorder);
type __atomic_fetch_and(type *ptr, type val, int memorder);
type __atomic_fetch_xor(type *ptr, type val, int memorder);
type __atomic_fetch_or(type *ptr, type val, int memorder);
type __atomic_fetch_nand(type *ptr, type val, int memorder);


//type __atomic_load (type *ptr,int memorder); ??

type __atomic_load_n (type *ptr,int memorder);
void __atomic_store_n (type *ptr, type val,int memorder);
type __atomic_exchange_n (type *ptr, type val,int memorder);
bool __atomic_compare_exchange_n (type *ptr, type *expected, type desired,bool weak,
								        int success_memorder,int failure_memorder);
//函数的用途从函数名可知，上面的带_n的后缀版本如果去掉_n就是不用提供memorder的seq_cst版本。





bool __atomic_test_and_set (void *ptr, int memorder);
void __atomic_clear (bool *ptr, int memorder);
void __atomic_thread_fence (int memorder);

bool __atomic_always_lock_free (size_t size, void *ptr);
bool __atomic_is_lock_free (size_t size, void *ptr);

//前面几个函数，意思比较明确。后面的两个函数，是判断系统上对于某个长度的对象是否会产生lock-free的原子操作。


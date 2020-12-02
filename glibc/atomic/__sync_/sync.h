
//----------------------------------------------------------------------------------------------------


//返回更新前的值,type可以是1,2,4或8字节长度的int类型
//--------------------------------------------------
type __sync_fetch_and_add (type *ptr, type value, ...);
type __sync_fetch_and_sub (type *ptr, type value, ...);
type __sync_fetch_and_or (type *ptr, type value, ...);
type __sync_fetch_and_and (type *ptr, type value, ...);
type __sync_fetch_and_xor (type *ptr, type value, ...);
type __sync_fetch_and_nand (type *ptr, type value, ...);
//
//返回更新后的值,type可以是1,2,4或8字节长度的int类型
//--------------------------------------------------
type __sync_add_and_fetch (type *ptr, type value, ...);
type __sync_sub_and_fetch (type *ptr, type value, ...);
type __sync_or_and_fetch (type *ptr, type value, ...);
type __sync_and_and_fetch (type *ptr, type value, ...);
type __sync_xor_and_fetch (type *ptr, type value, ...);
type __sync_nand_and_fetch (type *ptr, type value, ...);
//
//后面的可扩展参数(…)用来指出哪些变量需要memory barrier,
//因为目前gcc实现的是full barrier，所以可以略掉这个参数。

//----------------------------------------------------------------------------------------------------


//提供原子的比较和交换,如果ptr == oldval,就将newval写入ptr
//--------------------------------------------------
bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...);
type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...);
//
//第一个函数在相等并写入的情况下返回true.
//第二个函数在返回操作之前的值。

//----------------------------------------------------------------------------------------------------

//发出一个full barrier.
__sync_synchronize (...);

// 将*ptr设为value并返回*ptr操作之前的值。
type __sync_lock_test_and_set (type *ptr, type value, ...);

//将*ptr置0
void __sync_lock_release (type *ptr, ...);


//GCC实现了C++11之后（gcc版本要大于4.7），上面的__sync系列操作就不被推荐使用了，
//而基于C++11的新原子操作接口使用__atomic作为前缀。
//与__sync系列相比，__atomic系列函数最大的不同就是，需要指定内存序。


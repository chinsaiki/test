#include <pthread.h>

/* Robust mutex or not flags.  */
enum
{
  PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_STALLED_NP = PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_ROBUST,
  PTHREAD_MUTEX_ROBUST_NP = PTHREAD_MUTEX_ROBUST
};


//get and set the mutex robust attribute
//获取并设置互斥健壮属性
//锁的拥有者死掉后，其他线程试图获取锁将直接返回 EOWNERDEAD
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust);

//健壮的互斥锁：
//所有者死亡时会释放健壮的互斥锁，但是这也会带来高昂的间接费用。_NP此字符串中的表示此选项是非POSIX或不可移植的。
int pthread_mutexattr_setrobust_np(pthread_mutexattr_t *attr, int robust);


//对thread的mutex，可以通过方法 pthread_mutexattr_setrobust()来设置健壮性属性
//
//属性为： PTHREAD_MUTEX_ROBUST
//
//通过设置锁的上面D 属性，我们就改变了默认的行为。
//
//当一个锁的owner死掉后，其它线程再去lock这个锁的时候，不会被阻塞，而是通过返回值EOWNERDEAD来报告错误。
//
//那么你可以根据这个错误来进行处理：首先是应该调用pthread_mutex_consistent函数来恢复该锁的一致性，
//
//然后调用解锁pthread_mutex_unlock，
//
//接下来在调用加锁，这样该锁的行为就恢复正常了。
//
//
//如果上面这个函数在恢复锁的一致性时候没有成功，那么你只需要调用解锁函数就OK了，然后直接返回，而不要去调用加锁函数。
//
//那么接下来的线程在调用加锁函数的时候，会得到返回值ENOTRECOVERABLE，
//
//那么需要你做的就是调用pthread_mutex_destroy来destroy掉该锁，然后重新用锁的属性和锁的初始化函数来重新初始化该锁。
//
//
//上面的这些解决死锁方式比较适合在系统中只有一个锁的情况，如果系统的死锁是由于多把锁的资源互相等待而造成的，
//   那么这种解决方式无能为力。。。


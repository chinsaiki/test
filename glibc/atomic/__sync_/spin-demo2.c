typedef struct {
    volatile int lock;  // must be initialized to 0 before 1st call to atomic64_set
    volatile long long counter;
} atomic64_t;

static inline void atomic64_set(atomic64_t *v, long long i)
{
    // see https://attractivechaos.wordpress.com/2011/10/06/multi-threaded-programming-efficiency-of-locking/
    // for an explanation of __sync_lock_test_and_set
    while (__sync_lock_test_and_set(&v->lock, 1)) { // we don't have the lock, so busy wait until
        while (v->lock);                            // it is released (i.e. lock is set to 0)
    }                                               // by the holder via __sync_lock_release()
    // critical section
    v->counter = i;
    __sync_lock_release(&v->lock);
}


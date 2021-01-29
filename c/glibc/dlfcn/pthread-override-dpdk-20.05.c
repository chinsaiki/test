#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>
#include <unistd.h>

#include <pthread.h>


#define RTE_PRIORITY_LOG 101
#define RTE_PRIORITY_BUS 110
#define RTE_PRIORITY_CLASS 120
#define RTE_PRIORITY_LAST 65535

#define RTE_PRIO(prio) \
	RTE_PRIORITY_ ## prio

/**
 * Run function before main() with high priority.
 *
 * @param func
 *   Constructor function.
 * @param prio
 *   Priority number must be above 100.
 *   Lowest number is the first to run.
 */
#ifndef RTE_INIT_PRIO /* Allow to override from EAL */
#define RTE_INIT_PRIO(func, prio) \
static void __attribute__((constructor(RTE_PRIO(prio)), used)) func(void)
#endif

/**
 * Run function before main() with low priority.
 *
 * The constructor will be run after prioritized constructors.
 *
 * @param func
 *   Constructor function.
 */
#define RTE_INIT(func) \
	RTE_INIT_PRIO(func, LAST)

/**
 * Run after main() with low priority.
 *
 * @param func
 *   Destructor function name.
 * @param prio
 *   Priority number must be above 100.
 *   Lowest number is the last to run.
 */
#ifndef RTE_FINI_PRIO /* Allow to override from EAL */
#define RTE_FINI_PRIO(func, prio) \
static void __attribute__((destructor(RTE_PRIO(prio)), used)) func(void)
#endif

/**
 * Run after main() with high priority.
 *
 * The destructor will be run *before* prioritized destructors.
 *
 * @param func
 *   Destructor function name.
 */
#define RTE_FINI(func) \
	RTE_FINI_PRIO(func, LAST)

/*
 * this flag determines at run time if we override pthread
 * calls and map then to equivalent lthread calls
 * or of we call the standard pthread function
 */
static __thread int override;

typedef void *(*lthread_func_t) (void *);

/*
 * this structures contains function pointers that will be
 * initialised to the loaded address of the real
 * pthread library API functions
 */
struct pthread_lib_funcs {
int (*f_pthread_barrier_destroy)
	(pthread_barrier_t *);
int (*f_pthread_barrier_init)
	(pthread_barrier_t *, const pthread_barrierattr_t *, unsigned);
int (*f_pthread_barrier_wait)
	(pthread_barrier_t *);
int (*f_pthread_cond_broadcast)
	(pthread_cond_t *);
int (*f_pthread_cond_destroy)
	(pthread_cond_t *);
int (*f_pthread_cond_init)
	(pthread_cond_t *, const pthread_condattr_t *);
int (*f_pthread_cond_signal)
	(pthread_cond_t *);
int (*f_pthread_cond_timedwait)
	(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
int (*f_pthread_cond_wait)
	(pthread_cond_t *, pthread_mutex_t *);
int (*f_pthread_create)
	(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
int (*f_pthread_detach)
	(pthread_t);
int (*f_pthread_equal)
	(pthread_t, pthread_t);
void (*f_pthread_exit)
	(void *);
void * (*f_pthread_getspecific)
	(pthread_key_t);
int (*f_pthread_getcpuclockid)
	(pthread_t, clockid_t *);
int (*f_pthread_join)
	(pthread_t, void **);
int (*f_pthread_key_create)
	(pthread_key_t *, void (*) (void *));
int (*f_pthread_key_delete)
	(pthread_key_t);
int (*f_pthread_mutex_destroy)
	(pthread_mutex_t *__mutex);
int (*f_pthread_mutex_init)
	(pthread_mutex_t *__mutex, const pthread_mutexattr_t *);
int (*f_pthread_mutex_lock)
	(pthread_mutex_t *__mutex);
int (*f_pthread_mutex_trylock)
	(pthread_mutex_t *__mutex);
int (*f_pthread_mutex_timedlock)
	(pthread_mutex_t *__mutex, const struct timespec *);
int (*f_pthread_mutex_unlock)
	(pthread_mutex_t *__mutex);
int (*f_pthread_once)
	(pthread_once_t *, void (*) (void));
int (*f_pthread_rwlock_destroy)
	(pthread_rwlock_t *__rwlock);
int (*f_pthread_rwlock_init)
	(pthread_rwlock_t *__rwlock, const pthread_rwlockattr_t *);
int (*f_pthread_rwlock_rdlock)
	(pthread_rwlock_t *__rwlock);
int (*f_pthread_rwlock_timedrdlock)
	(pthread_rwlock_t *__rwlock, const struct timespec *);
int (*f_pthread_rwlock_timedwrlock)
	(pthread_rwlock_t *__rwlock, const struct timespec *);
int (*f_pthread_rwlock_tryrdlock)
	(pthread_rwlock_t *__rwlock);
int (*f_pthread_rwlock_trywrlock)
	(pthread_rwlock_t *__rwlock);
int (*f_pthread_rwlock_unlock)
	(pthread_rwlock_t *__rwlock);
int (*f_pthread_rwlock_wrlock)
	(pthread_rwlock_t *__rwlock);
pthread_t (*f_pthread_self)
	(void);
int (*f_pthread_setspecific)
	(pthread_key_t, const void *);
int (*f_pthread_spin_init)
	(pthread_spinlock_t *__spin, int);
int (*f_pthread_spin_destroy)
	(pthread_spinlock_t *__spin);
int (*f_pthread_spin_lock)
	(pthread_spinlock_t *__spin);
int (*f_pthread_spin_trylock)
	(pthread_spinlock_t *__spin);
int (*f_pthread_spin_unlock)
	(pthread_spinlock_t *__spin);
int (*f_pthread_cancel)
	(pthread_t);
int (*f_pthread_setcancelstate)
	(int, int *);
int (*f_pthread_setcanceltype)
	(int, int *);
void (*f_pthread_testcancel)
	(void);
int (*f_pthread_getschedparam)
	(pthread_t pthread, int *, struct sched_param *);
int (*f_pthread_setschedparam)
	(pthread_t, int, const struct sched_param *);
int (*f_pthread_yield)
	(void);
int (*f_pthread_setaffinity_np)
	(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);
int (*f_nanosleep)
	(const struct timespec *req, struct timespec *rem);
} _sys_pthread_funcs = {
	.f_pthread_barrier_destroy = NULL,
};


/*
 * this macro obtains the loaded address of a library function
 * and saves it.
 */
static void *__libc_dl_handle = RTLD_NEXT;

#define get_addr_of_loaded_symbol(name) do {				\
	char *error_str;						\
	_sys_pthread_funcs.f_##name = dlsym(__libc_dl_handle, (#name));	\
	error_str = dlerror();						\
	if (error_str != NULL) {					\
		fprintf(stderr, "%s\n", error_str);			\
	}								\
} while (0)


/*
 * The constructor function initialises the
 * function pointers for pthread library functions
 */
RTE_INIT(pthread_intercept_ctor)
{
	override = 0;
	/*
	 * Get the original functions
	 */
	get_addr_of_loaded_symbol(pthread_barrier_destroy);
	get_addr_of_loaded_symbol(pthread_barrier_init);
	get_addr_of_loaded_symbol(pthread_barrier_wait);
	get_addr_of_loaded_symbol(pthread_cond_broadcast);
	get_addr_of_loaded_symbol(pthread_cond_destroy);
	get_addr_of_loaded_symbol(pthread_cond_init);
	get_addr_of_loaded_symbol(pthread_cond_signal);
	get_addr_of_loaded_symbol(pthread_cond_timedwait);
	get_addr_of_loaded_symbol(pthread_cond_wait);
	get_addr_of_loaded_symbol(pthread_create);
	get_addr_of_loaded_symbol(pthread_detach);
	get_addr_of_loaded_symbol(pthread_equal);
	get_addr_of_loaded_symbol(pthread_exit);
	get_addr_of_loaded_symbol(pthread_getspecific);
	get_addr_of_loaded_symbol(pthread_getcpuclockid);
	get_addr_of_loaded_symbol(pthread_join);
	get_addr_of_loaded_symbol(pthread_key_create);
	get_addr_of_loaded_symbol(pthread_key_delete);
	get_addr_of_loaded_symbol(pthread_mutex_destroy);
	get_addr_of_loaded_symbol(pthread_mutex_init);
	get_addr_of_loaded_symbol(pthread_mutex_lock);
	get_addr_of_loaded_symbol(pthread_mutex_trylock);
	get_addr_of_loaded_symbol(pthread_mutex_timedlock);
	get_addr_of_loaded_symbol(pthread_mutex_unlock);
	get_addr_of_loaded_symbol(pthread_once);
	get_addr_of_loaded_symbol(pthread_rwlock_destroy);
	get_addr_of_loaded_symbol(pthread_rwlock_init);
	get_addr_of_loaded_symbol(pthread_rwlock_rdlock);
	get_addr_of_loaded_symbol(pthread_rwlock_timedrdlock);
	get_addr_of_loaded_symbol(pthread_rwlock_timedwrlock);
	get_addr_of_loaded_symbol(pthread_rwlock_tryrdlock);
	get_addr_of_loaded_symbol(pthread_rwlock_trywrlock);
	get_addr_of_loaded_symbol(pthread_rwlock_unlock);
	get_addr_of_loaded_symbol(pthread_rwlock_wrlock);
	get_addr_of_loaded_symbol(pthread_self);
	get_addr_of_loaded_symbol(pthread_setspecific);
	get_addr_of_loaded_symbol(pthread_spin_init);
	get_addr_of_loaded_symbol(pthread_spin_destroy);
	get_addr_of_loaded_symbol(pthread_spin_lock);
	get_addr_of_loaded_symbol(pthread_spin_trylock);
	get_addr_of_loaded_symbol(pthread_spin_unlock);
	get_addr_of_loaded_symbol(pthread_cancel);
	get_addr_of_loaded_symbol(pthread_setcancelstate);
	get_addr_of_loaded_symbol(pthread_setcanceltype);
	get_addr_of_loaded_symbol(pthread_testcancel);
	get_addr_of_loaded_symbol(pthread_getschedparam);
	get_addr_of_loaded_symbol(pthread_setschedparam);
	get_addr_of_loaded_symbol(pthread_yield);
	get_addr_of_loaded_symbol(pthread_setaffinity_np);
	get_addr_of_loaded_symbol(nanosleep);
}


/*
 * Enable/Disable pthread override
 * state
 *  0 disable
 *  1 enable
 */
void pthread_override_set(int state)
{
	override = state;
}


/*
 * Return pthread override state
 * return
 *  0 disable
 *  1 enable
 */
int pthread_override_get(void)
{
	return override;
}

/*
 * Create an lthread on the current scheduler
 * If there is no current scheduler on this pthread then first create one
 */
int
lthread_create(pthread_t **new_lt, int lcore_id,
		lthread_func_t fun, void *arg)
{
    printf("WARNING: create my thread.\n");
    fun(arg);
	return 0;
}


int
pthread_create(pthread_t *__restrict tid,
		const pthread_attr_t *__restrict attr,
		lthread_func_t func,
	       void *__restrict arg)
{
	if (override) {
		return lthread_create((pthread_t **)tid, 0,
				      func, arg);
	}
	return _sys_pthread_funcs.f_pthread_create(tid, attr, func, arg);
}

void *task1(void*arg) {
    
    printf("WARNING: my task.\n");
    
    pthread_exit(NULL);
}
int main (int argc, char *argv[]) {

    pthread_override_set(argc>1?atoi(argv[1]):1);

    pthread_t thread;

    pthread_create(&thread, NULL, task1, NULL);

    if(!pthread_override_get()) {
        pthread_join(thread, NULL);
    }

}


//https://github.com/MariaDB/server/blob/10.3/include/my_atomic.h
//http://en.cppreference.com/w/c/atomic

// Load
//Order must be one of MY_MEMORY_ORDER_RELAXED, MY_MEMORY_ORDER_CONSUME,
//MY_MEMORY_ORDER_ACQUIRE, MY_MEMORY_ORDER_SEQ_CST.
my_atomic_load32(int32_t *var);
my_atomic_load64(int64_t *var);
my_atomic_loadptr(void **var);
my_atomic_load32_explicit(int32_t *var, memory_order);
my_atomic_load64_explicit(int64_t *var, memory_order);
my_atomic_loadptr_explicit(void **var, memory_order);
{
    return *var;
}

//store
//Order must be one of MY_MEMORY_ORDER_RELAXED, MY_MEMORY_ORDER_RELEASE,
//MY_MEMORY_ORDER_SEQ_CST.
my_atomic_store32(&var, what);
my_atomic_store64(&var, what);
my_atomic_storeptr(&var, what);
my_atomic_store32_explicit(&var, what, memory_order);
my_atomic_store64_explicit(&var, what, memory_order);
my_atomic_storeptr_explicit(&var, what, memory_order);
{
    *var= what;
}

//fetch-and-store
//All memory orders are valid.
my_atomic_fas32(&var, what);
my_atomic_fas64(&var, what);
my_atomic_fasptr(&var, what);
my_atomic_fas32_explicit(&var, what, memory_order);
my_atomic_fas64_explicit(&var, what, memory_order);
my_atomic_fasptr_explicit(&var, what, memory_order);
{
    old= *var; var= *what; return old;
}

//ADD
//All memory orders are valid.
my_atomic_add32(&var, what);
my_atomic_add64(&var, what);
my_atomic_addptr(&var, what);
my_atomic_add32_explicit(&var, what, memory_order);
my_atomic_add64_explicit(&var, what, memory_order);
my_atomic_addptr_explicit(&var, what, memory_order);
{
    old= *var; *var+= what; return old;
}

//CAS
//succ - the memory synchronization ordering for the read-modify-write
//        operation if the comparison succeeds. All memory orders are valid.
//fail - the memory synchronization ordering for the load operation if the
//        comparison fails. Cannot be MY_MEMORY_ORDER_RELEASE or
//        MY_MEMORY_ORDER_ACQ_REL and cannot specify stronger ordering than succ.
//
//The weak form is allowed to fail spuriously, that is, act as if
//*var != *old even if they are equal. When a compare-and-exchange is in
//a loop, the weak version will yield better performance on some platforms.
//When a weak compare-and-exchange would require a loop and a strong one
//would not, the strong one is preferable.
my_atomic_cas32(&var, &old, new);
my_atomic_cas64(&var, &old, new);
my_atomic_casptr(&var, &old, new);
my_atomic_cas32_weak_explicit(&var, &old, new, succ, fail);
my_atomic_cas64_weak_explicit(&var, &old, new, succ, fail);
my_atomic_casptr_weak_explicit(&var, &old, new, succ, fail);
my_atomic_cas32_strong_explicit(&var, &old, new, succ, fail);
my_atomic_cas64_strong_explicit(&var, &old, new, succ, fail);
my_atomic_casptr_strong_explicit(&var, &old, new, succ, fail);
{
    if (*var == *old) { *var= new; return TRUE; }
    else { *old= *var; return FALSE; }
}

test1_this_is_OK() {

    thread1() {
        my_atomic_store64_explicit(&var, 1, MY_MEMORY_ORDER_RELAXED);
    }
    
    thread2() {
        if (my_atomic_load64_explicit(&var, MY_MEMORY_ORDER_RELAXED)) { 
            /* do something */
        }
    }
}

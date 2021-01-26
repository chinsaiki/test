/* Copyright (C) 2002-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <errno.h>
#include "pthreadP.h"
#include <atomic.h>


int
__pthread_detach (pthread_t th)
{
  struct pthread *pd = (struct pthread *) th;

  /* Make sure the descriptor is valid.  */
  if (INVALID_NOT_TERMINATED_TD_P (pd))
    /* Not a valid thread handle.  */
    return ESRCH;

  int result = 0;

  /* Mark the thread as detached.  */
  if (atomic_compare_and_exchange_bool_acq (&pd->joinid, pd, NULL))
    {
      /* There are two possibilities here.  First, the thread might
	 already be detached.  In this case we return EINVAL.
	 Otherwise there might already be a waiter.  The standard does
	 not mention what happens in this case.  */
      if (IS_DETACHED (pd))
	result = EINVAL;
    }
  else
    /* Check whether the thread terminated meanwhile.  In this case we
       will just free the TCB.  */
    if ((pd->cancelhandling & EXITING_BITMASK) != 0)
      /* Note that the code in __free_tcb makes sure each thread
	 control block is freed only once.  */
      __free_tcb (pd);

  return result;
}
weak_alias (__pthread_detach, pthread_detach)


/* Deallocate a thread's stack after optionally making sure the thread
   descriptor is still valid.  */
void __free_tcb (struct pthread *pd)
{
  /* The thread is exiting now.  */
  if (__builtin_expect (atomic_bit_test_set (&pd->cancelhandling,
                         TERMINATED_BIT) == 0, 1))
    {
      /* Remove the descriptor from the list.  */
      if (DEBUGGING_P && __find_in_stack_list (pd) == NULL)
    /* Something is really wrong.  The descriptor for a still
       running thread is gone.  */
    abort ();

      /* Free TPP data.  */
      if (__glibc_unlikely (pd->tpp != NULL))
    {
      struct priority_protection_data *tpp = pd->tpp;

      pd->tpp = NULL;
      free (tpp);
    }

      /* Queue the stack memory block for reuse and exit the process.  The
     kernel will signal via writing to the address returned by
     QUEUE-STACK when the stack is available.  */
      __deallocate_stack (pd);
    }
}

void
__deallocate_stack (struct pthread *pd)
{
  lll_lock (stack_cache_lock, LLL_PRIVATE);

  /* Remove the thread from the list of threads with user defined
     stacks.  */
  stack_list_del (&pd->list);

  /* Not much to do.  Just free the mmap()ed memory.  Note that we do
     not reset the 'used' flag in the 'tid' field.  This is done by
     the kernel.  If no thread has been created yet this field is
     still zero.  */
  if (__glibc_likely (! pd->user_stack))
    (void) queue_stack (pd);
  else
    /* Free the memory associated with the ELF TLS.  */
    _dl_deallocate_tls (TLS_TPADJ (pd), false);

  lll_unlock (stack_cache_lock, LLL_PRIVATE);
}

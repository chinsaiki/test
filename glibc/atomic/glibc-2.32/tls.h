/* Definition for thread-local data handling.  nptl/x86_64 version.
   Copyright (C) 2002-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#ifndef _TLS_H
#define _TLS_H	1


struct dtv_pointer
{
  void *val;                    /* Pointer to data, or TLS_DTV_UNALLOCATED.  */
  void *to_free;                /* Unaligned pointer, for deallocation.  */
};

/* Type for the dtv.  */
typedef union dtv
{
  size_t counter;
  struct dtv_pointer pointer;
} dtv_t;


/* Replacement type for __m128 since this file is included by ld.so,
   which is compiled with -mno-sse.  It must not change the alignment
   of rtld_savespace_sse.  */
typedef struct
{
  int i[4];
} __128bits;


typedef struct
{
  void *tcb;		/* Pointer to the TCB.  Not necessarily the
			   thread descriptor used by libpthread.  */
  dtv_t *dtv;
  void *self;		/* Pointer to the thread descriptor.  */
  int multiple_threads;
  int gscope_flag;
  uintptr_t sysinfo;
  uintptr_t stack_guard;
  uintptr_t pointer_guard;
  unsigned long int unused_vgetcpu_cache[2];
  /* Bit 0: X86_FEATURE_1_IBT.
     Bit 1: X86_FEATURE_1_SHSTK.
   */
  unsigned int feature_1;
  int __glibc_unused1;
  /* Reservation of some values for the TM ABI.  */
  void *__private_tm[4];
  /* GCC split stack support.  */
  void *__private_ss;
  /* The lowest address of shadow stack,  */
  unsigned long long int ssp_base;
  /* Must be kept even if it is no longer used by glibc since programs,
     like AddressSanitizer, depend on the size of tcbhead_t.  */
  __128bits __glibc_unused2[8][4] __attribute__ ((aligned (32)));

  void *__padding[8];
} tcbhead_t;

#endif	/* tls.h */


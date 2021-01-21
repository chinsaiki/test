/* include/urcu/config.h.  Generated from config.h.in by configure.  */
/* urcu/config.h.in. Manually generated for control over the contained defs. */

/* Defined when on a system that has memory fence instructions. */
#define CONFIG_RCU_HAVE_FENCE 1

/* Defined when on a system with futex support. */
#define CONFIG_RCU_HAVE_FUTEX 1

/* Enable SMP support. With SMP support enabled, uniprocessors are also
   supported. With SMP support disabled, UP systems work fine, but the
   behavior of SMP systems is undefined. */
#define CONFIG_RCU_SMP 1

/* Compatibility mode for i386 which lacks cmpxchg instruction. */
/* #undef CONFIG_RCU_COMPAT_ARCH */

/* Use the dmb instruction is available for use on ARM. */
/* #undef CONFIG_RCU_ARM_HAVE_DMB */

/* TLS provided by the compiler. */
#define CONFIG_RCU_TLS 1

/* clock_gettime() is detected. */
#define CONFIG_RCU_HAVE_CLOCK_GETTIME 1

/* Require the operating system to support the membarrier system call for
   default and bulletproof flavors. */
/* #undef CONFIG_RCU_FORCE_SYS_MEMBARRIER */

/* Enable internal debugging self-checks.
   Introduces a performance penalty. */
/* #undef CONFIG_RCU_DEBUG */

/* Expose multi-flavor support */
#define CONFIG_RCU_HAVE_MULTIFLAVOR 1

/* Enable extra debugging checks for lock-free hash table iterator
   traversal. */
/* #undef CONFIG_CDS_LFHT_ITER_DEBUG */

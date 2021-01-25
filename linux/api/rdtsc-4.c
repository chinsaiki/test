
#if defined __x86_64__
static inline uint64_t __tsc(void)
{
   unsigned int a, d;
   asm volatile ("rdtsc" : "=a" (a), "=d"(d));
   return ((unsigned long) a) | (((unsigned long) d) << 32);
}

#else
static inline uint64_t __tsc(void)
{
   uint64_t c;
   asm volatile ("rdtsc" : "=A" (c));
   return c;
}

#endif


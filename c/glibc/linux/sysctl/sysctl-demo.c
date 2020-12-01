#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sysctl.h>

//Glibc  does  not  provide  a wrapper for this system call; call it using syscall(2).  Or rather...  don't
//call it: use of this system call has long been discouraged, and it is so unloved that  it  is  likely  to
//disappear in a future kernel version.  Since Linux 2.6.24, uses of this system call result in warnings in
//the kernel log.  Remove it from your programs now; use the /proc/sys interface instead.
//
//This system call is available only if the kernel was configured with the CONFIG_SYSCTL_SYSCALL option.

int _sysctl(struct __sysctl_args *args );

#define OSNAMESZ 100

int
main(void)
{
   struct __sysctl_args args;
   char osname[OSNAMESZ];
   size_t osnamelth;
   int name[] = { CTL_KERN, KERN_OSTYPE };

   memset(&args, 0, sizeof(struct __sysctl_args));
   args.name = name;
   args.nlen = sizeof(name)/sizeof(name[0]);
   args.oldval = osname;
   args.oldlenp = &osnamelth;

   osnamelth = sizeof(osname);

   if (syscall(SYS__sysctl, &args) == -1) {
       perror("_sysctl");
       //_sysctl: Function not implemented 函数未执行,
       exit(EXIT_FAILURE);
   }
   printf("This machine is running %*s\n", osnamelth, osname);
   exit(EXIT_SUCCESS);
}


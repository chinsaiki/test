#include <sys/auxv.h>

//retrieve a value from the auxiliary vector
//	从辅助 vector 中检索一个值
//
//retrieve: 找回，取回，恢复
//auxiliary: 辅助设备，支援的，辅助人员
unsigned long getauxval(unsigned long type);


AT_BASE
//       The base address of the program interpreter (usually, the dynamic linker).

AT_BASE_PLATFORM
//       A string identifying the real platform; may differ from AT_PLATFORM (PowerPC only).

AT_CLKTCK
//       The   frequency   with   which   times(2)   counts.    This   value   can  also  be  obtained  via
//       sysconf(_SC_CLK_TCK).

AT_DCACHEBSIZE
//       The data cache block size.

AT_EGID
//       The effective group ID of the thread.

AT_ENTRY
//       The entry address of the executable.

AT_EUID
//       The effective user ID of the thread.

AT_EXECFD
//       File descriptor of program.

AT_EXECFN
//       Pathname used to execute program.

AT_FLAGS
//       Flags (unused).

AT_FPUCW
//       Used FPU control word (SuperH architecture only).  This gives some information about the FPU  ini‐
//       tialization performed by the kernel.

AT_GID 
//    The real group ID of the thread.

AT_HWCAP
//       A  pointer  to  a  multibyte mask of bits whose settings indicate detailed processor capabilities.
//       The contents of the bit mask are hardware dependent (for  example,  see  the  kernel  source  file
//       arch/x86/include/asm/cpufeature.h  for  details relating to the Intel x86 architecture).  A human-
//       readable version of the same information is available via /proc/cpuinfo.

AT_ICACHEBSIZE
//       The instruction cache block size.

AT_PAGESZ
//       The system page size (the same value returned by sysconf(_SC_PAGESIZE)).

AT_PHDR
//       The address of the program headers of the executable.

AT_PHENT
//       The size of program header entry.

AT_PHNUM
//       The number of program headers.

AT_PLATFORM
//       A pointer to a string that identifies the hardware platform that the program is running  on.   The
//       dynamic linker uses this in the interpretation of rpath values.

AT_RANDOM
//       The address of sixteen bytes containing a random value.

AT_SECURE
//       Has a nonzero value if this executable should be treated securely.  Most commonly, a nonzero value
//       indicates that the process is executing a set-user-ID or set-group-ID  program;  alternatively,  a
//       nonzero  value  may  be  triggered  by  a  Linux Security Module.  When this value is nonzero, the
//       dynamic linker disables the use of certain environment variables (see  ld-linux.so(8))  and  glibc
//       changes other aspects of its behavior.  (See also secure_getenv(3).)

AT_SYSINFO
//       The  entry point to the system call function in the VDSO.  Not present/needed on all architectures
//       (e.g., absent on x86-64).

AT_SYSINFO_EHDR
//       The address of a page containing the Virtual Dynamic Shared Object (VDSO) that the kernel  creates
//       in order to provide fast implementations of certain system calls.

AT_UCACHEBSIZE
//       The unified cache block size.

AT_UID 
//    The real user ID of the thread.


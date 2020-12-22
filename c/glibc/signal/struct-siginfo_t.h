
typedef struct
{
    int si_signo;		/* Signal number.  */
    int si_errno;		/* If non-zero, an errno value associated with
    			   this signal, as defined in <errno.h>.  */
    int si_code;		/* Signal code.  */

    union
      {
    int _pad[__SI_PAD_SIZE];

     /* kill().  */
    struct
      {
        __pid_t si_pid;	/* Sending process ID.  */
        __uid_t si_uid;	/* Real user ID of sending process.  */
      } _kill;

    /* POSIX.1b timers.  */
    struct
      {
        int si_tid;		/* Timer ID.  */
        int si_overrun;	/* Overrun count.  */
        sigval_t si_sigval;	/* Signal value.  */
      } _timer;

    /* POSIX.1b signals.  */
    struct
      {
        __pid_t si_pid;	/* Sending process ID.  */
        __uid_t si_uid;	/* Real user ID of sending process.  */
        sigval_t si_sigval;	/* Signal value.  */
      } _rt;

    /* SIGCHLD.  */
    struct
      {
        __pid_t si_pid;	/* Which child.  */
        __uid_t si_uid;	/* Real user ID of sending process.  */
        int si_status;	/* Exit value or signal.  */
        __sigchld_clock_t si_utime;
        __sigchld_clock_t si_stime;
      } _sigchld;

    /* SIGILL, SIGFPE, SIGSEGV, SIGBUS.  */
    struct
      {
        void *si_addr;	/* Faulting insn/memory ref.  */
      } _sigfault;

    /* SIGPOLL.  */
    struct
      {
        long int si_band;	/* Band event for SIGPOLL.  */
        int si_fd;
      } _sigpoll;

    /* SIGSYS.  */
    struct
      {
        void *_call_addr;	/* Calling user insn.  */
        int _syscall;	/* Triggering system call number.  */
        unsigned int _arch; /* AUDIT_ARCH_* of syscall.  */
      } _sigsys;
      } _sifields;
} siginfo_t /*__SI_ALIGNMENT*/;


/* X/Open requires some more fields with fixed names.  */
# define si_pid		_sifields._kill.si_pid
# define si_uid		_sifields._kill.si_uid
# define si_timerid	_sifields._timer.si_tid
# define si_overrun	_sifields._timer.si_overrun
# define si_status	_sifields._sigchld.si_status
# define si_utime	_sifields._sigchld.si_utime
# define si_stime	_sifields._sigchld.si_stime
# define si_value	_sifields._rt.si_sigval
# define si_int		_sifields._rt.si_sigval.sival_int
# define si_ptr		_sifields._rt.si_sigval.sival_ptr
# define si_addr	_sifields._sigfault.si_addr
# define si_band	_sifields._sigpoll.si_band
# define si_fd		_sifields._sigpoll.si_fd
# define si_call_addr 	_sifields._sigsys._call_addr
# define si_syscall	_sifields._sigsys._syscall
# define si_arch	_sifields._sigsys._arch



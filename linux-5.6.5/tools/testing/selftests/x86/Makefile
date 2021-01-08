# rongtao 2021年1月8日08:49:38

all:
	gcc check_initial_reg_state.c -o check_initial_reg_state.out
	gcc fsgsbase.c -pthread -o fsgsbase.out
	gcc fsgsbase_restore.c -o fsgsbase_restore.out
	gcc ioperm.c -o ioperm.out
	gcc iopl.c -o iopl.out
	gcc ldt_gdt.c -pthread -o ldt_gdt.out
	gcc mov_ss_trap.c -o mov_ss_trap.out
	gcc ptrace_syscall.c -DCAN_BUILD_64 -DCAN_BUILD_32 -lrt -ldl -lm -O2 -g -std=gnu99 -pthread -Wall -o ptrace_syscall.out
	gcc ptrace_syscall.c -m32 raw_syscall_helper_32.S -o ptrace_syscall_32.out
	gcc sigreturn.c -o sigreturn.out
	gcc single_step_syscall.c -o single_step_syscall.out
	gcc syscall_arg_fault.c -o syscall_arg_fault.out
	gcc syscall_nt.c -o syscall_nt.out
	gcc syscall_numbering.c -o syscall_numbering.out
	gcc sysret_rip.c -o sysret_rip.out
	gcc sysret_ss_attrs.c thunks.S -pthread -o sysret_ss_attrs.out
	gcc test_FCMOV.c -o test_FCMOV.out
	gcc test_FCOMI.c -lm -o test_FCOMI.out
	gcc test_FISTTP.c -lm -o test_FISTTP.out
	gcc test_mremap_vdso.c -o test_mremap_vdso.out
	gcc test_syscall_vdso.c -o test_syscall_vdso.out
	gcc test_vdso.c -ldl -o test_vdso.out
	gcc test_vsyscall.c -ldl -o test_vsyscall.out
	gcc trivial_64bit_program.c -o trivial_64bit_program.out
	gcc trivial_program.c -o trivial_program.out

clean:
	rm -f *.out

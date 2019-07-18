#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#if __WORDSIZE == 64
#define REG(reg) reg.orig_rax
#else
#define REG(reg) reg.orig_eax
#endif

const char* callname(long call);

int whitelist_syscall[] = {
    SCMP_SYS(read),		// 0
    SCMP_SYS(write),		// 1
    SCMP_SYS(open),		// 2
    SCMP_SYS(close),		// 3
    SCMP_SYS(stat),		// 4
    SCMP_SYS(fstat),		// 5
    SCMP_SYS(lstat),		// 6
    SCMP_SYS(lseek),		// 8
    SCMP_SYS(mmap),		// 9
    SCMP_SYS(mprotect),		// 10
    SCMP_SYS(munmap),		// 11
    SCMP_SYS(brk),		// 12
    SCMP_SYS(rt_sigaction),	// 13
    SCMP_SYS(rt_sigprocmask),	// 14
    SCMP_SYS(rt_sigreturn),	// 15
    SCMP_SYS(ioctl),		// 16
    SCMP_SYS(writev),		// 20
    SCMP_SYS(access),		// 21
    SCMP_SYS(pipe),		// 22
    SCMP_SYS(select),		// 23
    SCMP_SYS(sched_yield),	// 24
    SCMP_SYS(mremap),		// 25
    SCMP_SYS(madvise),		// 28
    SCMP_SYS(dup),		// 32
    SCMP_SYS(dup2),		// 33
    SCMP_SYS(nanosleep),	// 35
    SCMP_SYS(getpid),		// 39
    SCMP_SYS(socket),		// 41
    SCMP_SYS(clone),		// 56
    SCMP_SYS(exit),		// 60
    SCMP_SYS(kill),		// 62
    SCMP_SYS(fcntl),		// 72
    SCMP_SYS(getdents),		// 78
    SCMP_SYS(getcwd),		// 79
    SCMP_SYS(readlink),		// 89
    SCMP_SYS(gettimeofday),	// 96
    SCMP_SYS(getrlimit),	// 97
    SCMP_SYS(getrusage),	// 98
    SCMP_SYS(sysinfo),		// 99
    SCMP_SYS(getuid),		// 102
    SCMP_SYS(getgid),		// 104
    SCMP_SYS(geteuid),		// 107
    SCMP_SYS(getegid),		// 108
    SCMP_SYS(getppid),		// 110
    SCMP_SYS(getpgrp),		// 111
    SCMP_SYS(getgroups),	// 115
    SCMP_SYS(sigaltstack),	// 131
    SCMP_SYS(statfs),		// 137
    SCMP_SYS(sched_getparam),		// 143
    SCMP_SYS(sched_getscheduler),	// 145
    SCMP_SYS(sched_get_priority_max),	// 146
    SCMP_SYS(sched_get_priority_min),	// 147
    SCMP_SYS(modify_ldt),	// 154
    SCMP_SYS(prctl),		// 157
    SCMP_SYS(arch_prctl),	// 158
    SCMP_SYS(gettid),		// 186
    SCMP_SYS(futex),		// 202
    SCMP_SYS(sched_getaffinity),		// 204
    SCMP_SYS(set_tid_address),		// 218
    SCMP_SYS(restart_syscall),		// 219
    SCMP_SYS(timer_create),		// 222
    SCMP_SYS(timer_settime),		// 223
    SCMP_SYS(timer_delete),		// 226
    SCMP_SYS(clock_gettime),		// 228
    SCMP_SYS(clock_getres),		// 229
    SCMP_SYS(exit_group),		// 231
    SCMP_SYS(tgkill),		// 234
    SCMP_SYS(openat),		// 257
    SCMP_SYS(readlinkat),		// 267
    SCMP_SYS(faccessat),		// 269
    SCMP_SYS(ppoll),		// 271
    SCMP_SYS(set_robust_list),		// 273
    SCMP_SYS(timerfd_create),		// 283
    SCMP_SYS(dup3),		// 292
    SCMP_SYS(pipe2),		// 293
    SCMP_SYS(getrandom)		// 318
    //SCMP_SYS(clone3)		// 435
};

int main(int argc, char *argv[]) {
    pid_t pid = 0;
    int status;

    struct user_regs_struct regs;

    prctl(PR_SET_NO_NEW_PRIVS, 1);
    prctl(PR_SET_DUMPABLE, 0);

    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL); // default action: kill

    // build rules for whitelist of system calls
    for (int i = 0; i < sizeof(whitelist_syscall) / sizeof(int); i++) {
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    pid = fork();

    if (pid != 0) {
        while (waitpid(pid, &status, 0)) {
            if (WIFEXITED(status)) {
                fprintf(stderr, "terminated with code %d\n", WEXITSTATUS(status));
                break;
            } else if (WIFSIGNALED(status)) {
                fprintf(stderr, "terminated by signal %d\n", WTERMSIG(status));
                break;
            } else if (WIFSTOPPED(status)) {
                fprintf(stderr, "stopped by signal %d\n", WSTOPSIG(status));
                break;
            }

	    ptrace(PTRACE_GETREGS, pid, NULL, &regs);

            /*
	    if (disabled_syscalls[REG(regs)] == 1) {
		fprintf(stderr, "runtime error %s(%lld) from pid %d\n", callname(REG(regs)), REG(regs), pid);
		//ptrace(PTRACE_KILL, pid, NULL, NULL);
		kill(pid, SIGKILL);
	    }
            */

	    fprintf(stderr, "%s(%lld) from pid %d\n", callname(REG(regs)), REG(regs), pid);

	    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    } else if (pid == 0) {
        /*
	freopen("std.in", "r", stdin);
	freopen("std.out", "w", stdout);
	freopen("err.out", "a+", stderr);
        */

	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        seccomp_load(ctx);
	execl("/usr/bin/python3", "python3", "./test.py", NULL);

	exit(0);
    } else {
	perror("failed to fork");
    }

    return 0;
}

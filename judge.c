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
    SCMP_SYS(access),
    SCMP_SYS(arch_prctl),
    SCMP_SYS(brk),
    SCMP_SYS(close),
    SCMP_SYS(dup),
    SCMP_SYS(execve),
    SCMP_SYS(exit_group),
    SCMP_SYS(fcntl),
    SCMP_SYS(fstat),
    SCMP_SYS(futex),
    SCMP_SYS(getcwd),
    SCMP_SYS(getdents),
    SCMP_SYS(getegid),
    SCMP_SYS(geteuid),
    SCMP_SYS(getgid),
    SCMP_SYS(getpid),
    SCMP_SYS(getrandom),
    SCMP_SYS(getuid),
    SCMP_SYS(ioctl),
    SCMP_SYS(lseek),
    SCMP_SYS(lstat),
    SCMP_SYS(mmap),
    SCMP_SYS(mprotect),
    SCMP_SYS(munmap),
    SCMP_SYS(openat),
    SCMP_SYS(prlimit64),
    SCMP_SYS(read),
    SCMP_SYS(readlink),
    SCMP_SYS(rt_sigaction),
    SCMP_SYS(rt_sigprocmask),
    SCMP_SYS(set_robust_list),
    SCMP_SYS(set_tid_address),
    SCMP_SYS(sigaltstack),
    SCMP_SYS(stat),
    SCMP_SYS(sysinfo),
    SCMP_SYS(write)
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

            fprintf(stderr, "runtime error %s(%lld) from pid %d\n", callname(REG(regs)), REG(regs), pid);

            /*
	    if (disabled_syscalls[REG(regs)] == 1) {
		//ptrace(PTRACE_KILL, pid, NULL, NULL);
		kill(pid, SIGKILL);
	    }
            */

	    fprintf(stderr, "%s(%lld) from pid %d\n", callname(REG(regs)), REG(regs), pid);

	    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    } else if (pid == 0) {
	freopen("std.in", "r", stdin);
	freopen("std.out", "w", stdout);
	freopen("err.out", "a+", stderr);

	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        seccomp_load(ctx);
	execl("/usr/bin/python3", "python3", "./test.py", NULL);

	exit(0);
    } else {
	perror("failed to fork");
    }

    return 0;
}

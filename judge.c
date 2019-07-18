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

int disabled_syscalls[512] = {-1};

int main(int argc, char *argv[]) {
    pid_t pid = 0;
    int status;

    struct user_regs_struct regs;

    prctl(PR_SET_NO_NEW_PRIVS, 1);
    prctl(PR_SET_DUMPABLE, 0);

    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL); // default action: kill

    // setup basic whitelist
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigreturn), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);

    pid = fork();

    disabled_syscalls[78] = 1;

    if (pid != 0) {
	printf("parent %d\n", pid);

	while(waitpid(pid, &status, 0) && !WIFEXITED(status)) {
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
	freopen("std.in", "r", stdin);
	freopen("std.out", "w", stdout);
	freopen("err.out", "a+", stderr);

	printf("child %d\n", pid);

	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

	execl("/usr/bin/python3", "python3", "./test.py", NULL);

	exit(0);
    } else {
	perror("failed to fork");
    }

    return 0;
}

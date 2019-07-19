#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "callname.h"
#include "whitelist.h"


int main(int argc, char *argv[]) {
    pid_t pid = 0;
    int status;

    struct user_regs_struct regs;

    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    prctl(PR_SET_DUMPABLE, 0);

    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL); // default action: kill

    // build rules for whitelist of system calls
    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    pid = fork();

    if (pid != 0) {
        while (waitpid(pid, &status, 0)) {
            if (WIFEXITED(status)) {
                fprintf(stderr, "terminated with code %d\n", WEXITSTATUS(status));
                break;
            } else if (WIFSIGNALED(status)) {
                if (WTERMSIG(status) == 31) {
                    fprintf(stderr, "killed by system call violation\n");
                } else {
                    fprintf(stderr, "terminated by signal %d\n", WTERMSIG(status));
                }
                break;
            }	  

            ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESECCOMP);
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);

	    fprintf(stderr, "%s(%lld) from pid %d\n", callname(REG(regs)), REG(regs), pid);

	    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    } else if (pid == 0) {
        FILE *fp_in = freopen("std.in", "r", stdin);
	FILE *fp_out = freopen("std.out", "w", stdout);
	FILE *fp_error = freopen("err.out", "a+", stderr);

	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        //seccomp_load(ctx);

	//execl("/usr/bin/java", "/usr/bin/java", "test/Test", NULL);
	execl("/usr/bin/mono", "/usr/bin/mono", "test/test.exe", NULL);
	//execl("/usr/bin/python3", "python", "test/test.py", NULL);
	//execl("test/testc.out", "test/test.out", NULL);
	//execl("test/testcpp.out", "test/test.out", NULL);

        seccomp_release(ctx);

        fclose(fp_in);
        fclose(fp_out);
        fclose(fp_error);

	exit(0);
    } else {
	perror("failed to fork");
    }

    return 0;
}

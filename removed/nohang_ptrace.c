#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <seccomp.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "whitelist.h"

static scmp_filter_ctx seccomp_context;

int main(int argc, char *argv[]) {
    pid_t cpid;

    int wstatus;

    struct user_regs_struct regs;

    /*
    seccomp_context = seccomp_init(SCMP_ACT_KILL);

    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }
    */

    cpid = fork();

    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);

    if (cpid == -1) {
        fprintf(stderr, "failed to create a process\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        fprintf(stderr, "child: %d\n", getpid());

        ptrace(PTRACE_TRACEME, 0, 0, 0);

        //seccomp_load(seccomp_context);

        execl("/bin/uname", "/bin/uname", "-a", NULL);
        fprintf(stderr, "failed to replace process with %s\n", "/bin/ls");
    } else {
        fprintf(stderr, "parent: %d\n", getpid());

        while (1) {
            if (waitpid(cpid, &wstatus, WNOHANG) == -1) {
                fprintf(stderr, "failed to wait child process (%d)\n", cpid);
                break;
            }

            ptrace(PTRACE_GETREGS, cpid, NULL, &regs);
            fprintf(stderr, "syscall(%lld)\n", regs.orig_rax);
            ptrace(PTRACE_SYSCALL, cpid, 0, 0);

            if (WIFEXITED(wstatus)) {
                fprintf(stderr, "exited with status %d\n", WEXITSTATUS(wstatus));
                break;
            } else if (WIFSIGNALED(wstatus)) {
                fprintf(stderr, "killed by signal %d\n", WTERMSIG(wstatus));
            } else if (WIFSTOPPED(wstatus)) {
                fprintf(stderr, "stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                fprintf(stderr, "resumed by signal SIGCONT\n");
            }
        }
    }

    return 0;
}

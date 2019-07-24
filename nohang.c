#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "whitelist.h"

static void sig_chld(int dummy) {
    pid_t pid;
    int status;

    (void) dummy;

    if ((pid = waitpid(-1, &status, WNOHANG)) == -1) {
        fprintf(stderr, "waitpid failed\n");
        return;
    }

    fprintf(stderr, "sig pid %d\n", pid);

    if (WIFEXITED(status)) {
        exit(WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[]) {
    pid_t cpid;

    int wstatus;

    struct user_regs_struct regs;

    struct sigaction sa_chld;

    sa_chld.sa_handler = sig_chld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        fprintf(stderr, "failed to install sigaction\n");
        return 1;
    }

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

        execl("/bin/uname", "/bin/uname", "-a", NULL);
        fprintf(stderr, "failed to replace process with %s\n", "/bin/ls");
    } else {
        fprintf(stderr, "parent: %d\n", getpid());

        while (1) {
            ptrace(PTRACE_GETREGS, cpid, NULL, &regs);
            fprintf(stderr, "syscall(%lld)\n", regs.orig_rax);
            ptrace(PTRACE_SYSCALL, cpid, 0, 0);
        }
    }

    return 0;
}

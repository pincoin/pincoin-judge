#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <seccomp.h>

#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "whitelist.h"

static scmp_filter_ctx seccomp_context;

int main(int argc, char *argv[]) {
    int wstatus;

    pid_t cpid;

    char **args = malloc(sizeof(char *) * argc);

    struct user_regs_struct regs;

    fprintf(stderr, "online judge runner\n");

    /* 1. make sure if args provided */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s requires args\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 2. use seccomp */
    /* NOTE
     * build rules for whitelist of system calls
     * and socket(AF_UNIX, ...
     */
    seccomp_context = seccomp_init(SCMP_ACT_KILL);

    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1,
            SCMP_A0(SCMP_CMP_EQ, AF_UNIX));

    /* 3. create a child process */
    cpid = fork();

    if (cpid == -1) {
        fprintf(stderr, "failed to create a process\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        fprintf(stderr, "child: %d\n", cpid);

        /* 1. make command */
        for (int i = 0; i < argc - 1; i++) {
            args[i] = strdup(argv[i + 1]);
        }
        args[argc - 1] = NULL;

        /* 2. set up process option */
        /* NOTE
         * PR_SET_NO_NEW_PRIVS = 1: ensures the process doesn't gain privileges
         * PR_SET_DUMPABLE = 0: doesn't produce core dump
         */
        prctl(PR_SET_NO_NEW_PRIVS, 1);
        prctl(PR_SET_DUMPABLE, 0);

        /* 3. use ptrace */
        ptrace(PTRACE_TRACEME, 0, 0, 0); /* enter ptrace-stop */

        /* 4. use seccomp */
        seccomp_load(seccomp_context);

        /* 5. run a program */
        /* NOTE
         * executes the program and enters ptrace-stop.
         * causes child to stop and send a signal to parent
         * parent can now switch to PTRACE_SYSCALL
         */
        execv(args[0], args);
        fprintf(stderr, "failed to replace process with %s\n", args[0]);
    } else {
        fprintf(stderr, "parent: %d\n", cpid);

        /* NOTE
         * PTRACE_0_TRACESECCOMP: stop the tracee when a SECCOMP_RET_TRACE rule is triggered
         * PTRACE_O_TRACESYSGOOD: set bit 7 in the signal number (SIGTRAP|0x80) when delivering system call traps
         */
        ptrace(PTRACE_SETOPTIONS, cpid, 0, PTRACE_O_TRACESECCOMP);

        do {
            /* 1. restart the stopped tracee childe */
            ptrace(PTRACE_SYSCALL, cpid, 0, 0);

            /* 2. waitpid */
            if (waitpid(cpid, &wstatus, 0) == -1) {
                fprintf(stderr, "failed to wait child termination\n");
                exit(EXIT_FAILURE);
            }

            /* 3. retrieve syscall info of child */
            ptrace(PTRACE_GETREGS, cpid, NULL, &regs);
            fprintf(stderr, "syscall(%lld)\n", regs.orig_rax);

            /* 4. check wait status */
            if (wstatus>>8 == (SIGTRAP | (PTRACE_EVENT_SECCOMP<<8))) {
                fprintf(stderr, "killed by syscall violation\n");
            } else if (WIFEXITED(wstatus)) {
                fprintf(stderr, "exited with status %d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                fprintf(stderr, "killed by signal %d\n", WTERMSIG(wstatus));
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        /* clean up */
        seccomp_release(seccomp_context);

        free(args);

        exit(EXIT_SUCCESS);
    }

    return 0;
}

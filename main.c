#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <seccomp.h>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/prctl.h>  
#include <sys/socket.h>

#include "whitelist.h"

int main(int argc, char *argv[]) {
    pid_t  pid;
    int status;

    char **args = malloc(sizeof(char *) * argc);

    long orig_rax;
    long rax;
    int stop = 0;
    struct user_regs_struct regs;

    scmp_filter_ctx ctx;

    /* 1. make sure if argv provide */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s requires arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 2. process control */
    /* NOTE
     * PR_SET_NO_NEW_PRIVS = 1: ensures the process does not gain privileges
     * PR_SET_DUMPABLE = 0: does not produce core dump
     */
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0);
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);

    /* 3. create a new process */
    pid = fork();
    if (pid == -1) {
        fprintf(stderr, "failed to create a new process\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* 1. make a NULL-terminated command */
        for (int i = 0; i < argc - 1; i++) {
            args[i] = strdup(argv[i + 1]);
        }
        args[argc - 1] = NULL;

        /* 2. use ptrace */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        /* 3. use seccomp sandbox */
        /* 3-1. initalize seccomp */
        ctx = seccomp_init(SCMP_ACT_KILL);

        /* 3-2. allow functions from whitelist */
        for (int i = 0; i < size_of_whitelist_syscall; i++) {
            seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
        }

        /* 3-3. allow socket function for unix socket */
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1,
                SCMP_A0(SCMP_CMP_EQ, AF_UNIX));

        /* 3-4. load seccomp rules */
        seccomp_load(ctx);

        /* 4. exec */
        syscall(59, args[0], args, NULL);
        fprintf(stderr, "failed to replace process with %s\n", args[0]);
        exit(EXIT_FAILURE);
    } else {
        while (1) {
            /* 1. wait for child process non-blocking */
            waitpid(pid, &status, WNOHANG);

            /* 2. check if child terminated */
            if (WIFEXITED(status)) {
                fprintf(stderr, "exited with status %d\n", WEXITSTATUS(status));
                break;
            } else if (WIFSIGNALED(status) && WTERMSIG(status) == 31) {
                fprintf(stderr, "syscall violation\n");
                break;
            }

            /* 3. retrieve child process tracee's USER area */
            /* NOTE
             * - process has USER, DATA, TEXT area
             * - accumulator register: %AX(16bit), %EAX(32bit), %RAX(64bit)
             * - ORIG_RAX has syscall number
             */
            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);

            if (orig_rax > -1) {
                if (stop == 1) {
                    fprintf(stderr, "syscall(%ld)\n", orig_rax);

                    ptrace(PTRACE_GETREGS, pid, NULL, &regs);

                    stop = 0;
                } else {
                    /* NOTE
                     * - syscall result value is in %RAX
                     */
                    rax  = ptrace(PTRACE_PEEKUSER, pid, 8 * RAX, NULL); /* MUST pop even if RAX is not used. */
                    /* fprintf(stderr, " return with %ld\n", rax); */

                    stop = 1;
                }
            }

            /* 4. invoke system call to resume child tracee */
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    }
    return 0;
}

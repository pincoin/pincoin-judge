//code7.c
//gcc code7.c -o code7 -lseccomp
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>     /* prctl */
#include <linux/seccomp.h> /* seccomp's constants */
#include <seccomp.h>
#include <stdlib.h>
#include <unistd.h>
 
#define longsize 8
 
char buffer[100];
 
 
int main()
{
    pid_t  pid;
    int rv;
    long orig_rax;
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
    long length;
    long addr;
    int insyscall = 0;
    struct user_regs_struct regs;
 
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW); // default action: allow
 
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 1, SCMP_A2(SCMP_CMP_EQ, 60));
 
    seccomp_load(ctx);
    prctl(PR_SET_NO_NEW_PRIVS, 1);
    pid = fork();
    if(pid == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        syscall(59,  cmd ,argv, env);
    }
    else
    {
        while(1)
        {
            wait(&rv);
            if(WIFEXITED(rv)){
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);
             
            if(orig_rax == 1)
            {
                if(insyscall == 0)
                {
                    printf("Syscall number: %d\n", orig_rax);
                    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                    printf("Write called with 0x%lx, 0x%lx, 0x%lx\n", regs.rdi, regs.rsi, regs.rdx);
                    addr = regs.rsi;
                    length = regs.rdx;
                    insyscall = 1;
                }
                else
                {
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


//code6.c
//gcc code6.c -o code6
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
#include <stdlib.h>
#include <unistd.h>
 
#define longsize 8
 
char buffer[100];
 
int reverse(char *str)
{
    int i, j;
    char temp;
    for(i = 0, j = strlen(str) - 2; i <= j; ++i, --j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}
 
int getdata(pid_t pid, char *buf, long addr, long length)
{
    int i;
    union {
        long val;
        char chars[longsize];
    } value;
    for(i=0; i < length/longsize + 1; i++)
    {
        value.val = ptrace(PTRACE_PEEKDATA, pid, (void*)(addr + i*8), NULL);
        memcpy(buf + i*8, value.chars, 8);
    }
    buf[length+1] = '\0';
    return 0;
}
 
int putdata(pid_t pid, char *buf, long addr, long length)
{
    int i;
    union {
        long val;
        char chars[longsize];
    }value;
    for(i=0; i<length/longsize + 1; i++)
    {
        memcpy(value.chars, buffer + i*8, 8);
        ptrace(PTRACE_POKEDATA, pid, (void*)(addr + i*8), value.val);
    }
    return 0;
}
 
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
    pid = fork();
    if(pid == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        syscall(59,  cmd ,argv, env);
        exit(0);
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
                    printf("write called with 0x%lx, 0x%lx, 0x%lx\n", regs.rdi, regs.rsi, regs.rdx);
                    addr = regs.rsi;
                    length = regs.rdx;
                    getdata(pid, buffer, addr, length);
                    reverse(buffer);
                    putdata(pid, buffer, addr, length);
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

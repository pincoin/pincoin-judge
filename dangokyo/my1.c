#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

int main(int argc, char *argv[]) {
    pid_t  pid;
    int status;
    long orig_rax;
    long value;
    int insyscall = 0;
    struct user_regs_struct regs;
    pid = fork();

    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/cat", "/bin/cat", "flag", NULL);
        exit(0);
    } else {
        while(1) {
            waitpid(pid, &status, WNOHANG);
            if (WIFEXITED(status)){
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);

            if (orig_rax == 1) {
                if (insyscall == 0)
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#if __WORDSIZE == 64
#define REG(reg) reg.orig_rax
#else
#define REG(reg) reg.orig_eax
#endif

const char* callname(long call);

int main(int argc, char *argv[]) {
    char* chargs[argc];
    int i = 0;

    pid_t pid = 0;
    int status;
    struct user_regs_struct regs;

    if (argc == 1) {
	exit(0);
    }

    // construct command
    while (i < argc - 1) {
	chargs[i] = argv[i+1];
	i++;
    }
    chargs[i] = NULL;

    pid = fork();

    if (pid != 0) {
	printf("parent %d\n", pid);

	while(waitpid(pid, &status, 0) && ! WIFEXITED(status)) {
	    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	    fprintf(stderr, "system call %s from pid %d\n", callname(REG(regs)), pid);
	    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	}

    } else if (pid == 0) {
	printf("child %d\n", pid);
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	execvp(chargs[0], chargs);
    } else {
	perror("failed to fork");
    }

    return 0;
}

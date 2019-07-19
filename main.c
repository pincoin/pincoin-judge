#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/user.h>

#ifdef PTRACE
    #include <sys/ptrace.h>

    #if __WORDSIZE == 64
        #define REG(reg) reg.orig_rax
    #else
        #define REG(reg) reg.orig_eax
    #endif
#endif


int main(int argc, char *argv[]) {
    pid_t cpid;
    int wstatus;
    struct user_regs_struct regs;

    cpid = fork();

    if (cpid == -1) {
        perror("failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {                /* Code executed by child */
        printf("child %d\n", cpid);

        #ifdef PTRACE
            ptrace(PTRACE_TRACEME, 0, 0, 0);
        #endif

        prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
        prctl(PR_SET_DUMPABLE, 0);


        execl("/bin/uname", "uname", "-a", NULL);
	//execl("/usr/bin/java", "/usr/bin/java", "test/Test", NULL);
	//execl("/usr/bin/mono", "/usr/bin/mono", "test/test.exe", NULL);
	//execl("/usr/bin/python3", "python", "test/test.py", NULL);
	//execl("test/testc.out", "test/test.out", NULL);
	//execl("test/testcpp.out", "test/test.out", NULL);
	//execl("/usr/bin/nodejs", "/usr/bin/nodejs", "test/test.js", NULL);

        /*
        char *args[] = {"/bin/ls", "-r", "-t", "-l", NULL};
        execv("/bin/ls", args);
        */

        exit(EXIT_SUCCESS);
    } else {                        /* Code executed by parent */
        printf("parent %d\n", cpid);

        do {
            if (waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED) == -1) {
                exit(EXIT_FAILURE);
            }

            #ifdef PTRACE
                ptrace(PTRACE_SETOPTIONS, cpid, 0, PTRACE_O_TRACESECCOMP);
                ptrace(PTRACE_GETREGS, cpid, NULL, &regs);
                fprintf(stderr, "syscall(%lld)\n", REG(regs));
                ptrace(PTRACE_SYSCALL, cpid, NULL, NULL);
            #endif

            if (WIFEXITED(wstatus)) {
                fprintf(stdout, "exited, status=%d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                fprintf(stdout, "killed by signal %d\n", WTERMSIG(wstatus));
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        exit(EXIT_SUCCESS);
    }
    return 0;
}


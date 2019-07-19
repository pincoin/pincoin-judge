#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <seccomp.h>
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

#include "whitelist.h"

int main(int argc, char *argv[]) {
    pid_t cpid;
    int wstatus;

    struct user_regs_struct regs;

    struct timespec tstart={0, 0}, tend={0, 0};

    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);

    FILE *fp_in = freopen("std.in", "r", stdin);
    FILE *fp_out = freopen("std.out", "w", stdout);
    FILE *fp_error = freopen("err.log", "a+", stderr);

    // build rules for whitelist of system calls
    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(ctx, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    cpid = fork();

    if (cpid == -1) {
        perror("failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {                /* Code executed by child */
        #ifdef PTRACE
            ptrace(PTRACE_TRACEME, 0, 0, 0);
        #endif

        prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
        prctl(PR_SET_DUMPABLE, 0);

        seccomp_load(ctx);

	execl("/usr/bin/java", "/usr/bin/java", "test/Test", NULL);
	//execl("/usr/bin/mono", "/usr/bin/mono", "test/test.exe", NULL);
	//execl("/usr/bin/python3", "python", "test/test.py", NULL);
	//execl("test/testc.out", "test/test.out", NULL);
	//execl("test/testcpp.out", "test/test.out", NULL);
	//execl("/usr/bin/nodejs", "/usr/bin/nodejs", "test/test.js", NULL);

        perror("failed to replace the process");
        exit(EXIT_FAILURE);
    } else {                        /* Code executed by parent */
        clock_gettime(CLOCK_MONOTONIC, &tstart);

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
                fprintf(stderr, "exited, status=%d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                fprintf(stderr, "killed by signal %d\n", WTERMSIG(wstatus));
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        clock_gettime(CLOCK_MONOTONIC, &tend);

        // 1. check time
        printf("%.5f ms\n", (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec)
                    - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)) * 1000);

        // 2. check memory

        // 3. check timeout (in do-while loop)

        // 4. check disk space

        // 5. runtime error or match (maybe in Python)

        fflush(fp_out);
        fflush(fp_error);

        fclose(fp_in);
        fclose(fp_out);
        fclose(fp_error);

        exit(EXIT_SUCCESS);
    }
    return 0;
}


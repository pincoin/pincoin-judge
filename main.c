#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/user.h>

#include "main.h"

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

    struct timespec tstart={0, 0}, tend={0, 0};

    FILE *fp_in = freopen("std.in", "r", stdin);
    FILE *fp_out = freopen("std.out", "w", stdout);
    FILE *fp_error = freopen("err.log", "a+", stderr);

    char **command = malloc(sizeof(char *) * argc);

    if (argc == 1) {
        fprintf(stderr, "%s: filename args\n", argv[0]);
        fprintf(stderr, "Run program\n");
        exit(EXIT_FAILURE);
    }

    command = build_command(argc, argv);

    seccomp_context = seccomp_init(SCMP_ACT_KILL);

    build_seccomp_rules();

    cpid = fork();

    if (cpid == -1) {
        fprintf(stderr, "failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {                /* Code executed by child */
        run_program(command);

        fprintf(stderr, "failed to replace the process");
        exit(EXIT_FAILURE);
    } else {                        /* Code executed by parent */
        clock_gettime(CLOCK_MONOTONIC, &tstart);

        wait_program(cpid);

        clock_gettime(CLOCK_MONOTONIC, &tend);

        // 1. check time
        printf("%.5f ms\n", (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec)
                    - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)) * 1000);

        // 2. check memory

        // 3. check timeout (in do-while loop)

        // 4. check disk space

        // 5. runtime error or match (maybe in Python)

        /* clean up tasks */
        seccomp_release(seccomp_context);

        for (int i = 0; i < argc; i++) {
            free(command[i]);
        }

        fflush(fp_out);
        fflush(fp_error);

        fclose(fp_in);
        fclose(fp_out);
        fclose(fp_error);

        exit(EXIT_SUCCESS);
    }
    return 0;
}

static char **build_command(int argc, char **argv) {
    char **command = malloc(sizeof(char *) * argc);

    /* copy argv to command which is NULL-terminated */
    for (int i = 0; i < argc - 1; i++) {
        command[i] = strdup(argv[i + 1]);
    }
    command[argc - 1] = NULL;

    return command;
}

static void build_seccomp_rules() {
    /* build rules for whitelist of system calls */
    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    /* socket(AF_UNIX, ... */
    seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1,
            SCMP_A0(SCMP_CMP_EQ, AF_UNIX));
}

static void run_program(char **command) {
    #ifdef PTRACE
        ptrace(PTRACE_TRACEME, 0, 0, 0);
    #endif

    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    prctl(PR_SET_DUMPABLE, 0);

    seccomp_load(seccomp_context);

    execv(command[0], command);
}

static void wait_program(pid_t pid) {
    int wstatus;
    struct user_regs_struct regs;

    do {
        if (waitpid(pid, &wstatus, WUNTRACED | WCONTINUED) == -1) {
            exit(EXIT_FAILURE);
        }

        #ifdef PTRACE
            ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESECCOMP);
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "syscall(%lld)\n", REG(regs));
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        #endif

        if (WIFEXITED(wstatus)) {
            fprintf(stderr, "exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "killed by signal %d\n", WTERMSIG(wstatus));
        }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
}

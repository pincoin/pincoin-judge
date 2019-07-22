#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <seccomp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/user.h>

#include "main.h"

#ifdef USE_PTRACE
    #include <sys/ptrace.h>

    #if __WORDSIZE == 64
        #define REG(reg) reg.orig_rax
    #else
        #define REG(reg) reg.orig_eax
    #endif
#endif

#include "whitelist.h"

#ifdef TRACE_MEMORY
    #define PATH_MAX 2048
#endif

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

    #ifdef USE_SECCOMP
        seccomp_context = seccomp_init(SCMP_ACT_KILL);
        build_seccomp_rules();
    #endif

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
        #ifdef USE_SECCOMP
            seccomp_release(seccomp_context);
        #endif

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

#ifdef USE_SECCOMP
static void build_seccomp_rules() {
    /* build rules for whitelist of system calls */
    for (int i = 0; i < size_of_whitelist_syscall; i++) {
        seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, whitelist_syscall[i], 0);
    }

    /* socket(AF_UNIX, ... */
    seccomp_rule_add(seccomp_context, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1,
            SCMP_A0(SCMP_CMP_EQ, AF_UNIX));
}
#endif

static void run_program(char **command) {
    #ifdef USE_PTRACE
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
        prctl(PR_SET_DUMPABLE, 0);
    #endif

    #ifdef USE_SECCOMP
        seccomp_load(seccomp_context);
    #endif

    execv(command[0], command);
}

static void wait_program(pid_t pid) {
    int wstatus;

    #ifdef USE_PTRACE
        struct user_regs_struct regs;
    #endif

    #ifdef TRACE_MEMORY
        char pid_status_file_path[PATH_MAX];
        snprintf(pid_status_file_path, PATH_MAX, "/proc/%d/status", pid);
        fprintf(stderr, "%s\n", pid_status_file_path);
    #endif

    do {
        /* loop that restarts wait if interrupted by a signal
         * 
         * parent process is blocked until the child finishes by calling `wait`.
         *
         * Bitwise options
         * WNOHANG: `wait` returns immediately if the status of a child is not available.
         * WUNTRACED: `wait` also returns if a child has stopped (but not traced via ptrace).
         * WCONTINUED: `wait` also returns if a stopped child has been resumed by delivery of SIGCONT.
         * 
         * EINTR: function was interrupted by a signal
         */
        if (waitpid(pid, &wstatus, 0) == -1 && errno != EINTR) {
            exit(EXIT_FAILURE);
        }

        #ifdef USE_PTRACE
            ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESECCOMP);
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "syscall(%lld)\n", REG(regs));
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        #endif

        #ifdef TRACE_MEMORY
            trace_memory(pid_status_file_path);
        #endif

        if (WIFEXITED(wstatus)) {
            fprintf(stderr, "exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "killed by signal %d\n", WTERMSIG(wstatus));
        } /* else if (WIFSTOPPED(wstatus)) {
            fprintf(stderr, "stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if (WIFCONTINUED(wstatus)) {
            fprintf(stderr, "continued\n");
        } */
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
}

#ifdef TRACE_MEMORY
static int trace_memory(char *pid_status_file_path) {
    char *line;
    char *vmsize;
    char *vmpeak;
    char *vmrss;
    char *vmhwm;

    size_t len;

    FILE *f;

    vmsize = NULL;
    vmpeak = NULL;
    vmrss = NULL;
    vmhwm = NULL;
    line = malloc(128);
    len = 128;

    f = fopen(pid_status_file_path, "r");
    if (!f) return 1;

    /* Read memory size data from /proc/pid/status */
    while (!vmsize || !vmpeak || !vmrss || !vmhwm) {
        if (getline(&line, &len, f) == -1) {
            /* Some of the information isn't there, die */
            return 1;
        }

        if (!strncmp(line, "VmPeak:", 7)) {         /* Find VmPeak */
            vmpeak = strdup(&line[7]);
        } else if (!strncmp(line, "VmSize:", 7)) {  /* Find VmSize */
            vmsize = strdup(&line[7]);
        } else if (!strncmp(line, "VmRSS:", 6)) {   /* Find VmRSS */
            vmrss = strdup(&line[7]);
        } else if (!strncmp(line, "VmHWM:", 6)) {   /* Find VmHWM */
            vmhwm = strdup(&line[7]);
        }
    }

    free(line);

    fclose(f);

    /* Get rid of " kB\n"*/
    len = strlen(vmsize);
    vmsize[len - 4] = 0;
    len = strlen(vmpeak);
    vmpeak[len - 4] = 0;
    len = strlen(vmrss);
    vmrss[len - 4] = 0;
    len = strlen(vmhwm);
    vmhwm[len - 4] = 0;

    /* Output results to stderr */
    fprintf(stderr, "%s\t%s\t%s\t%s\n", vmsize, vmpeak, vmrss, vmhwm);

    free(vmpeak);
    free(vmsize);
    free(vmrss);
    free(vmhwm);

    /* Success */
    return 0;
}
#endif

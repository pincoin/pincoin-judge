#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t cpid;
    int wstatus;

    cpid = fork();

    if (cpid == -1) {
        perror("failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        fprintf(stdout, "child pid: %d\n", cpid);

        sleep(2);

        exit(EXIT_SUCCESS);
    } else {
        fprintf(stdout, "parent pid: %d\n", cpid);

        do {
            if (waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED) == -1) {
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(wstatus)) {
                fprintf(stdout, "exited, status=%d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                fprintf(stdout, "killed by signal %d\n", WTERMSIG(wstatus));
            } else if (WIFSTOPPED(wstatus)) {
                fprintf(stdout, "stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                fprintf(stdout, "continued\n");
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        exit(EXIT_SUCCESS);
    }
    return 0;
}


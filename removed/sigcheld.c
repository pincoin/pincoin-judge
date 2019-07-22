#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

void proc_exit() {
    int wstatatus;
    pid_t pid;

    while (1) {
        pid = wait(&wstatatus);
        if (pid == 0)
            return;
        else if (pid == -1)
            return;
        else
            printf("exit");
    }
}

int main (int argc, char *argv[])
{
    signal (SIGCHLD, proc_exit);

    switch (fork()) {
        case -1:
            perror("main: fork");
            exit(0);
        case 0:
            printf("I'm alive (temporarily)\n");
            exit(rand());
        default:
            pause();
    }
}

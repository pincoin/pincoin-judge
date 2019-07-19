#include <stdio.h>
#include <unistd.h>

int main(void) {
    char *args[] = {"/bin/ls", "-r", "-t", "-l", NULL};
    execv("/bin/ls", args);
    printf("not executed");
    return 0;
}

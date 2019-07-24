//code1.c
//gcc code1.c -o code1
#include<stdio.h>
#include<unistd.h>
#include<sys/syscall.h>

int main()
{
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
    syscall(59, cmd, argv, env);
    return 0;
}

//code2.c
//gcc code2.c -o code2 -lseccomp
#include<stdio.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/prctl.h>
#include<linux/seccomp.h> 

int main()
{
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
    prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
    syscall(59, cmd, argv, env);
    return 0;
}

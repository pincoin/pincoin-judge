//code4.c
//gcc code4.c -o code4 -lseccomp
#include<stdio.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/prctl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<linux/seccomp.h> 
#include<seccomp.h>

int main()
{
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
    pid_t pid;
    int rv;
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW); // default action: Allow
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 0);
    seccomp_load(ctx);

    pid = fork();
    if(pid==0){
        syscall(59, cmd, argv, env);
    }
    else
    {
        waitpid(pid, &rv, 0);
    }
    return 0;
}

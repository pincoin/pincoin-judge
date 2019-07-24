#include<stdio.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/prctl.h>
#include<linux/seccomp.h> 
#include<seccomp.h>
 
int main()
{
    char *argv[]={"/bin/cat", "flag", NULL};
    char *env[]={NULL};
    char cmd[20] = "/bin/cat";
 
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW); // default action: Allow
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 0);
    seccomp_load(ctx);
 
    syscall(59, cmd, argv, env);
    return 0;
}

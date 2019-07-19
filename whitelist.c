#include <seccomp.h>


int whitelist_syscall[] = {
    SCMP_SYS(access), // check file access at
    SCMP_SYS(arch_prctl),
    SCMP_SYS(brk),
    SCMP_SYS(clock_getres),
    SCMP_SYS(clone),
    SCMP_SYS(close),
    SCMP_SYS(dup),
    SCMP_SYS(epoll_create1),
    SCMP_SYS(eventfd2),
    SCMP_SYS(execve),
    SCMP_SYS(exit_group),
    SCMP_SYS(fcntl),
    SCMP_SYS(fstat),
    SCMP_SYS(ftruncate), // check
    SCMP_SYS(futex),
    SCMP_SYS(getcwd),
    SCMP_SYS(getdents),
    SCMP_SYS(getegid),
    SCMP_SYS(geteuid),
    SCMP_SYS(getgid),
    SCMP_SYS(getpid),
    SCMP_SYS(getrandom),
    SCMP_SYS(getuid),
    SCMP_SYS(ioctl),
    SCMP_SYS(kill), // do kill
    SCMP_SYS(lseek),
    SCMP_SYS(lstat), // check file access
    SCMP_SYS(mmap),
    SCMP_SYS(mprotect),
    SCMP_SYS(munmap),
    SCMP_SYS(openat), // check file access at
    SCMP_SYS(pipe2),
    SCMP_SYS(poll),
    SCMP_SYS(prctl), // do prctl
    SCMP_SYS(prlimit64),
    SCMP_SYS(read), // cehck file access
    SCMP_SYS(readlink), // check file access at
    SCMP_SYS(rt_sigaction),
    SCMP_SYS(rt_sigprocmask),
    SCMP_SYS(sched_getaffinity),
    SCMP_SYS(sched_yield),
    SCMP_SYS(set_robust_list),
    SCMP_SYS(set_tid_address),
    SCMP_SYS(sigaltstack),
    SCMP_SYS(stat), // check file access
    SCMP_SYS(statfs),
    SCMP_SYS(sysinfo),
    SCMP_SYS(tgkill), // do kill
    SCMP_SYS(unlink), // access eperm
    SCMP_SYS(write)
};

int size_of_whitelist_syscall = sizeof(whitelist_syscall) / sizeof(int);


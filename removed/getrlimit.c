#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    struct rlimit rlim;

    getrlimit(RLIMIT_NPROC, &rlim);
    printf("# of PROC MAX : %lu : %lu\n", rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_NOFILE, &rlim);
    printf("# of FILE MAX : %lu : %lu\n", rlim.rlim_cur, rlim.rlim_max);

    getrlimit(RLIMIT_CPU, &rlim);

    if (rlim.rlim_cur == RLIM_INFINITY) {
        printf("UNLIMIT\n");
    }
}

#include <stdio.h>
#include <time.h>

int main() {
    struct timespec tstart={0,0}, tend={0,0};

    clock_gettime(CLOCK_MONOTONIC, &tstart);

    for (int i = 0; i < 10000000; i++) {
        
    }

    clock_gettime(CLOCK_MONOTONIC, &tend);

    printf("some_long_computation took about %.5f miliseconds\n",
            (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
            ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec))*1000);
}

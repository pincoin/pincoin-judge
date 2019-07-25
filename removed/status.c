#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *fp;
    int data = 0, stack = 0;
    char buf[2048];
    char *vm;

    fp = fopen("/proc/1/status", "r");

    if (fp == NULL)
        exit(EXIT_FAILURE);
    fread(buf, 2047, 1, fp);
    buf[2047] = '\0';
    fclose(fp);

    vm = strstr(buf, "VmData:");
    if (vm) {
        sscanf(vm, "%*s %d", &data);
    }
    vm = strstr(buf, "VmStk:");
    if (vm) {
        sscanf(vm, "%*s %d", &stack);
    }

    printf("data(%d) + stack(%d) = total(%d)\n", data, stack, data + stack);

    exit(EXIT_SUCCESS);

    return 0;
}

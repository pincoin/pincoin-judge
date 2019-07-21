#ifndef __main_h__
#define __main_h__

#include <seccomp.h>

static scmp_filter_ctx seccomp_context;

static char **build_command(int argc, char **argv);
static void build_seccomp_rules();
static void run_program(char **command);
static void wait_program(pid_t pid);

#endif

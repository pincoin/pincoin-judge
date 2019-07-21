#ifndef __main_h__
#define __main_h__

#ifdef DEBUG_SECCOMP
#include <seccomp.h>

static scmp_filter_ctx seccomp_context;

static void build_seccomp_rules();
#endif

static char **build_command(int argc, char **argv);
static void run_program(char **command);
static void wait_program(pid_t pid);

#endif

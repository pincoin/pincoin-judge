#ifndef __main_h__
#define __main_h__

static void chld_sig_handler(int signo);
static char **build_command(int argc, char **argv);
static void run_program(char **command);
static void watch_program(pid_t pid);

#ifdef USE_SECCOMP
#include <seccomp.h>
static scmp_filter_ctx seccomp_context;

static void build_seccomp_rules();
#endif

#ifdef TRACE_MEMORY
static int trace_memory(char *pid_status_file_path);
#endif

#endif

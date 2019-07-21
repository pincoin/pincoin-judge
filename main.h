#ifndef __main_h__
#define __main_h__

static char **build_command(int argc, char **argv);
static void build_seccomp_rules(scmp_filter_ctx context);
static void run_program(scmp_filter_ctx context, char **command);

#endif

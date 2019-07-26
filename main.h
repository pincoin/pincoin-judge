#ifndef __main_h__
#define __main_h__

#define PID_STATUS_PATH_MAX 2048
#define PID_STATUS_FILE_MAX 2048

#define MEMORY_LIMIT 500
#define TIME_LIMIT 2

static void run_solution(int argc, char *argv[]);
static void watch_program(pid_t pid);

#endif
